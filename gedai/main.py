from fastapi import FastAPI, HTTPException
import requests
import os
from pathlib import Path
from fastapi.staticfiles import StaticFiles

from gedai.schematic_parser import parse_geda_schematic

app = FastAPI()

# --- Configuration (Could use environment variables) ---
# Example: Using Ollama running locally
OLLAMA_API_URL = "http://localhost:11434/api/generate"
OLLAMA_MODEL = "llama3" # Or whatever model you're using

# Example: Using Hugging Face Inference API
# HF_API_TOKEN = os.environ.get("HF_API_TOKEN")
# HF_MODEL_ID = "mistralai/Mistral-7B-Instruct-v0.2"
# hf_client = InferenceClient(model=HF_MODEL_ID, token=HF_API_TOKEN)

@app.get("/")
async def hello_chat():
    """
    Simple endpoint to check if the gedai server is running.
    """
    return {"message": "Hello from gEDA-AI!"}

from pathlib import Path
from fastapi.staticfiles import StaticFiles

# Mount examples directory
app.mount("/examples", StaticFiles(directory="examples"), name="examples")

@app.get("/load_schematic/{schematic_path:path}")
async def load_schematic(schematic_path: str):
    """Load a .sch file from the examples directory"""
    full_path = Path("examples") / schematic_path
    if not full_path.exists():
        raise HTTPException(status_code=404, detail="Schematic not found")
    
    with open(full_path, "r", encoding="utf-8") as f:
        content = f.read()
    
    return {
        "filename": schematic_path,
        "content": content,
        "components": parse_geda_schematic(content)
    }


@app.post("/generate_symbol")
async def generate_symbol(description: str):
    """
    Endpoint to generate a gEDA symbol based on a description.
    (This is just a placeholder - the prompt and processing need refinement)
    """
    prompt = f"Generate a gEDA schematic symbol text block for: {description}. Only output the gEDA text block."

    try:
        # --- Option 1: Calling local Ollama ---
        response = requests.post(OLLAMA_API_URL, json={
            "model": OLLAMA_MODEL,
            "prompt": prompt,
            "stream": False
        })
        response.raise_for_status()
        result = response.json()
        generated_text = result.get("response", "").strip()

        # --- Option 2: Calling Hugging Face Inference API ---
        # result = hf_client.text_generation(prompt=prompt, max_new_tokens=500)
        # generated_text = result.strip()

        if not generated_text:
            raise HTTPException(status_code=500, detail="LLM returned empty response")

        print(f"LLM Output:\n{generated_text}")

        return {"symbol_text": generated_text}

    except requests.exceptions.RequestException as e:
        print(f"Error calling LLM API: {e}")
        raise HTTPException(status_code=503, detail=f"Error communicating with LLM: {e}")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")
        raise HTTPException(status_code=500, detail=f"Internal server error: {e}")
