"""Exercise the installed helper protocol over a real loopback HTTP server."""
import json
import os
from pathlib import Path
import subprocess
import sys
import threading
import unittest
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

ROOT = Path(__file__).resolve().parents[1]


class Handler(BaseHTTPRequestHandler):
    requests = []

    def log_message(self, *args):
        pass

    def reply(self, data, status=200):
        raw = json.dumps(data).encode()
        self.send_response(status)
        self.send_header('Content-Type', 'application/json')
        self.send_header('Content-Length', str(len(raw)))
        self.end_headers()
        self.wfile.write(raw)

    def do_GET(self):
        self.requests.append((self.path, None, self.headers.get('Authorization')))
        if self.path == '/api/tags':
            self.reply({'models': [{'name': 'fixture:latest'}]})
        else:
            self.reply({'data': [{'id': 'fixture-model'}]})

    def do_POST(self):
        request = json.loads(self.rfile.read(int(self.headers['Content-Length'])))
        self.requests.append((self.path, request, self.headers.get('Authorization')))
        text = 'Reply for: ' + request['messages'][-1]['content']
        if self.path == '/api/chat':
            self.reply({'message': {'role': 'assistant', 'content': text}})
        elif self.path == '/unauthorized/chat/completions':
            self.reply({'error': 'do-not-echo-this-secret'}, 401)
        else:
            self.reply({'choices': [{'message': {'role': 'assistant', 'content': text}}]})


class ChatHTTP(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.server = ThreadingHTTPServer(('127.0.0.1', 0), Handler)
        cls.thread = threading.Thread(target=cls.server.serve_forever, daemon=True)
        cls.thread.start()
        cls.base = f'http://127.0.0.1:{cls.server.server_port}'

    @classmethod
    def tearDownClass(cls):
        cls.server.shutdown()
        cls.server.server_close()
        cls.thread.join()

    def run_helper(self, provider, operation='chat', endpoint=None):
        request = dict(provider=provider, model='fixture-model', operation=operation,
                       api_key='fixture-only-key', env_file='',
                       endpoint=self.base + (endpoint or ('/api/chat' if provider == 'ollama'
                                                          else '/v1/chat/completions')),
                       question='two resistors: µΩ "quoted"', schematic='v 20261019 2\n',
                       history=[])
        result = subprocess.run([sys.executable, str(ROOT/'gedai/chat_backend.py')],
                                input=json.dumps(request), text=True, capture_output=True,
                                env={'PATH': os.defpath}, timeout=10, cwd='/tmp')
        return result, request

    def test_chat_all_providers(self):
        for provider in ('ollama', 'openai', 'deepseek', 'openrouter', 'compatible'):
            with self.subTest(provider=provider):
                result, request = self.run_helper(provider)
                self.assertEqual(result.returncode, 0, result.stderr)
                self.assertEqual(result.stdout, 'Reply for: ' + request['question'])
                _, payload, authorization = Handler.requests[-1]
                self.assertEqual(authorization, 'Bearer fixture-only-key')
                self.assertIn(request['schematic'], payload['messages'][0]['content'])

    def test_models_all_providers(self):
        for provider in ('ollama', 'openai', 'deepseek', 'openrouter'):
            with self.subTest(provider=provider):
                result, _ = self.run_helper(provider, 'models')
                self.assertEqual(result.returncode, 0, result.stderr)
                self.assertIn('fixture', result.stdout)
                self.assertIsNone(Handler.requests[-1][1])

    def test_http_error_is_actionable_and_redacted(self):
        result, _ = self.run_helper('openai', endpoint='/unauthorized/chat/completions')
        self.assertEqual(result.returncode, 1)
        self.assertIn('401', result.stderr)
        self.assertNotIn('do-not-echo', result.stderr)
        self.assertNotIn('fixture-only-key', result.stderr)


if __name__ == '__main__':
    unittest.main()
