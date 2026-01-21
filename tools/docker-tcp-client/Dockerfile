FROM python:3.11-slim

WORKDIR /app

COPY tcp_client.py .

RUN chmod +x tcp_client.py

ENV PYTHONUNBUFFERED=1

ENTRYPOINT ["python", "tcp_client.py"]
