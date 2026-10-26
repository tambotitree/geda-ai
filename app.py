from fastapi import FastAPI, HTTPException, Header, Request
from fastapi.responses import HTML_Response
from google.oauth2 import id_token
from google.auth.transport import requests as auth_requests
import requests
import os

app = FastAPI()
OLLAMA_URL = "http://127.0.0.1:11434/api/generate"

async def verify_google_token(request: Request):
    auth_header = request.headers.get("Authorization")
    if not auth_header or not auth_header.startswith("Bearer "):
        raise HTTPException(status_code=401, detail="Unauthorized: Missing Bearer Token")
    
    token = auth_header.split(" ")[1]
    try:
        # This verifies the token is signed by Google
        return id_token.verify_oauth2_token(token, auth_requests.Request())
    except Exception as e:
        raise HTTPException(status_code=401, detail=f"Invalid Token: {str(e)}")

@app.get("/", response_class=HTML_Response)
async def read_root():
    return """
    <html>
        <head><title>gEDA-ai Interface</title></head>
        <body style="font-family: sans-serif; padding: 2em;">
            <h1>gEDA-ai Web Interface</h1>
            <textarea id="prompt" style="width: 100%; height: 100px;"></textarea><br><br>
            <button onclick="ask()">Ask gEDA-ai</button>
            <div id="response" style="margin-top: 20px; white-space: pre-wrap; border: 1px solid #ccc; padding: 10px;"></div>
            <script>
                async function ask() {
                    const prompt = document.getElementById('prompt').value;
                    const resDiv = document.getElementById('response');
                    resDiv.innerText = "Thinking...";
                    const response = await fetch('/ask', {
                        method: 'POST',
                        headers: {'Content-Type': 'application/json'},
                        body: JSON.stringify({prompt: prompt})
                    });
                    const data = await response.json();
                    resDiv.innerText = data.response || data.error;
                }
            </script>
        </body>
    </html>
    """

@app.post("/ask")
async def ask_model(payload: dict, request: Request):
    # Optional: Enable this to enforce auth at the code level 
    # (Cloud Run already enforces it at the infrastructure level)
    # await verify_google_token(request)
    
    try:
        r = requests.post(OLLAMA_URL, json={
            "model": "geda-custom",
            "prompt": payload['prompt'],
            "stream": False
        })
        return r.json()
    except Exception as e:
        return {"error": str(e)}