FROM python:3.13-alpine
WORKDIR /app
COPY services/omnis_platform.py /app/omnis_platform.py
USER 10001:10001
EXPOSE 8080
ENTRYPOINT ["python", "/app/omnis_platform.py"]
