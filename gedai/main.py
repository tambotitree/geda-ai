from fastapi import FastAPI, HTTPException
import requests
import os
from pathlib import Path
from fastapi.staticfiles import StaticFiles
from dotenv import load_dotenv

from gedai.schematic_parser import parse_geda_schematic

# Load environment variables
load_dotenv()

app = FastAPI()

# OpenRouter configuration
OPENROUTER_API_URL = "https://openrouter.ai/api/v1/chat/completions"
OPENROUTER_MODEL = "deepseek-ai/deepseek-chat"  # Using DeepSeek model
OPENROUTER_API_KEY = os.getenv("OPENROUTER_API_KEY")

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
    Generate a gEDA symbol using DeepSeek via OpenRouter
    """
    prompt = f"""You are an expert in gEDA schematic design. 
    Generate a gEDA schematic symbol text block for: {description}. 
    Only output the valid gEDA text format, nothing else.
    Example format:
    v 20200609 2
    P 1000 1000 2000 1000 3 0 0
    {{
    T 1500 1500 5 10 1 0 0 0 1
    value=10k
    }}
    """

    try:
        headers = {
            "Authorization": f"Bearer {OPENROUTER_API_KEY}",
            "Content-Type": "application/json"
        }
        
        payload = {
            "model": OPENROUTER_MODEL,
            "messages": [{"role": "user", "content": prompt}],
            "temperature": 0.3,
            "max_tokens": 500
        }

        response = requests.post(OPENROUTER_API_URL, 
                               headers=headers, 
                               json=payload)
        response.raise_for_status()
        
        result = response.json()
        generated_text = result['choices'][0]['message']['content'].strip()

        # Validate the output contains gEDA format markers
        if not any(marker in generated_text for marker in ["v ", "P ", "T ", "{"]):
            raise HTTPException(status_code=500, detail="Invalid gEDA format generated")

        print(f"Generated symbol:\n{generated_text}")
        return {"symbol_text": generated_text}

    except requests.exceptions.RequestException as e:
        print(f"OpenRouter API error: {e}")
        raise HTTPException(status_code=503, detail=f"OpenRouter API error: {e}")
    except KeyError as e:
        print(f"Malformed API response: {e}")
        raise HTTPException(status_code=502, detail="Malformed API response")
    except Exception as e:
        print(f"Unexpected error: {e}")
        raise HTTPException(status_code=500, detail=f"Internal server error: {e}")
