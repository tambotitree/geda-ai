# Use Ollama's official image with GPU support
FROM ollama/ollama:latest

# Set environment variables
ENV PYTHONUNBUFFERED=1 \
    PYTHONDONTWRITEBYTECODE=1

# Install Python and system dependencies
RUN apt-get update && apt-get install -y \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Install Python web dependencies
RUN pip3 install fastapi uvicorn requests google-auth

# Cloud Run expects traffic on 8080
EXPOSE 8080

# Copy application code and entrypoint
COPY . .
RUN chmod +x entrypoint.sh

ENTRYPOINT ["./entrypoint.sh"]