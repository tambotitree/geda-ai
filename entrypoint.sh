#!/bin/bash

# Start Ollama in the background
# We use the default port 11434 internally
ollama serve > /dev/null 2>&1 &

# Wait for server to start
sleep 5

# Create a custom model with MoE constraints
# Note: 'num_experts' or routing is usually model-defined, 
# but we can set top_k in parameters if supported by the GGUF.
echo "FROM gemma2
PARAMETER num_gpu 40
PARAMETER top_k 4" > Modelfile

ollama create geda-custom -f Modelfile

# Start the Python Web Interface on port 8080 for Cloud Run
echo "Starting Python Web Interface..."
exec uvicorn app:app --host 0.0.0.0 --port 8080