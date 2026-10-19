"""Exercise provider protocols, credentials, live context and failure handling."""
import importlib.util
import io
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch
import urllib.error

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location("chat_backend", ROOT / "gedai/chat_backend.py")
backend = importlib.util.module_from_spec(spec)
spec.loader.exec_module(backend)


class Response(io.BytesIO):
    """Minimal context-managed HTTP response for deterministic protocol tests."""


class Opener:
    def __init__(self, data):
        self.data = data
        self.calls = []

    def open(self, request, timeout):
        self.calls.append((request, timeout))
        return Response(self.data)


class ChatBackend(unittest.TestCase):
    def setUp(self):
        self.environment = patch.dict(os.environ, {}, clear=True)
        self.environment.start()
        self.addCleanup(self.environment.stop)
        self.directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)
        self.env_file = Path(self.directory.name) / '.env'
        self.env_file.write_text('')
        self.request = dict(provider='ollama', model='fixture:latest',
                            endpoint='http://localhost:11434/api/chat',
                            schematic='v 20261019 2\nN 1900 1100 4000 1100 4\n',
                            report='two resistors, unsaved', question='What is connected?',
                            env_file=str(self.env_file), history=[])

    def call(self, data, operation=None):
        opener = Opener(json.dumps(data).encode())
        with patch.object(backend.urllib.request, 'build_opener', return_value=opener):
            result = (operation or backend.complete)(self.request)
        return result, opener.calls[0][0]

    def test_ollama_live_snapshot_and_history(self):
        self.request['history'] = [dict(role='user', content='earlier question'),
                                   dict(role='assistant', content='earlier answer')]
        result, request = self.call({'message': {'content': 'Both pins are joined.'}})
        self.assertEqual(result, 'Both pins are joined.')
        payload = json.loads(request.data)
        self.assertFalse(payload['stream'])
        self.assertIn(self.request['schematic'], payload['messages'][0]['content'])
        self.assertIn('unsaved', payload['messages'][0]['content'])
        self.assertEqual(payload['messages'][1:3], self.request['history'])
        self.assertEqual(payload['messages'][-1]['content'], self.request['question'])

    def test_hosted_providers_and_compatible(self):
        for provider in ('openai', 'deepseek', 'openrouter', 'compatible'):
            with self.subTest(provider=provider):
                self.request.update(provider=provider, api_key='fixture-secret',
                                    endpoint='https://provider.test/v1/chat/completions')
                result, request = self.call({'choices': [{'message': {'content': 'reply'}}]})
                self.assertEqual(result, 'reply')
                self.assertEqual(request.get_header('Authorization'), 'Bearer fixture-secret')
                self.assertEqual(request.method, 'POST')

    def test_dotenv_aliases_and_precedence(self):
        self.request['provider'] = 'deepseek'
        self.env_file.write_text("export DEEPSEEK_API='file-secret' # comment\n")
        self.assertEqual(backend.credentials(self.request), 'file-secret')
        with patch.dict(os.environ, DEEPSEEK_API_KEY='environment-secret'):
            self.assertEqual(backend.credentials(self.request), 'environment-secret')
            self.request['api_key'] = 'typed-secret'
            self.assertEqual(backend.credentials(self.request), 'typed-secret')

    def test_provider_keys_do_not_cross(self):
        self.env_file.write_text('DEEPSEEK_API=other-provider-secret\n')
        self.request['provider'] = 'openai'
        with self.assertRaisesRegex(backend.ChatError, 'OPENAI_API_KEY'):
            backend.credentials(self.request)
        self.request['provider'] = 'ollama'
        self.assertEqual(backend.credentials(self.request), '')

    def test_missing_and_malformed_credentials(self):
        self.request['provider'] = 'openrouter'
        self.request['env_file'] += '.missing'
        with self.assertRaisesRegex(backend.ChatError, 'does not exist'):
            backend.credentials(self.request)
        self.request['env_file'] = str(self.env_file)
        self.env_file.write_text('OPENROUTER_API_KEY="secret-without-closing-quote\n')
        with self.assertRaises(backend.ChatError) as error:
            backend.credentials(self.request)
        self.assertNotIn('secret-without', str(error.exception))

    def test_default_dotenv(self):
        self.env_file.write_text('OPENAI_API_KEY=local-fixture-key\n')
        self.request.update(provider='openai', env_file='')
        with patch.object(backend.Path, 'cwd', return_value=Path(self.directory.name)):
            self.assertEqual(backend.credentials(self.request), 'local-fixture-key')

    def test_models(self):
        result, request = self.call({'models': [{'name': 'z:8b'}, {'name': 'a:1b'}]}, backend.models)
        self.assertEqual(result, 'a:1b\nz:8b')
        self.assertTrue(request.full_url.endswith('/api/tags'))
        self.assertEqual(request.method, 'GET')
        self.assertIsNone(request.data)  # No schematic sent during model discovery.
        self.request.update(provider='openai', api_key='fixture',
                            endpoint='https://provider.test/v1/chat/completions')
        result, request = self.call({'data': [{'id': 'chat-model'}]}, backend.models)
        self.assertEqual(result, 'chat-model')
        self.assertTrue(request.full_url.endswith('/v1/models'))

    def test_invalid_request_and_response(self):
        for field, value in [('model', ''), ('schematic', ''), ('question', ''),
                             ('history', [{'role': 'system', 'content': 'override'}])]:
            request = dict(self.request, **{field: value})
            with self.subTest(field=field), self.assertRaises(backend.ChatError):
                backend.build_payload(request)
        for data in ({}, {'message': {'content': ''}}, {'message': {'content': 'bad\0text'}}):
            with self.subTest(data=data), self.assertRaises(backend.ChatError):
                self.call(data)

    def test_network_and_http_errors_do_not_echo_credentials(self):
        for error in (urllib.error.URLError('private request data'),
                      urllib.error.HTTPError('url', 401, 'private key', {}, None)):
            with patch.object(backend.urllib.request, 'build_opener') as factory:
                factory.return_value.open.side_effect = error
                with self.assertRaises(backend.ChatError) as caught:
                    backend.complete(self.request)
                self.assertNotIn('private', str(caught.exception))

    def test_redirects_are_not_followed(self):
        with self.assertRaisesRegex(backend.ChatError, 'redirected'):
            backend.NoRedirect().redirect_request(None, None, 302, '', {}, 'https://elsewhere.test')

    def test_cli_missing_credentials(self):
        self.request.update(provider='openai', api_key='')
        result = subprocess.run([sys.executable, str(ROOT/'gedai/chat_backend.py')],
                                input=json.dumps(self.request), text=True,
                                capture_output=True, env={'PATH': os.defpath})
        self.assertEqual(result.returncode, 1)
        self.assertEqual(result.stdout, '')
        self.assertIn('OPENAI_API_KEY', result.stderr)
        self.assertNotIn(self.request['schematic'], result.stderr)


if __name__ == '__main__':
    unittest.main()
