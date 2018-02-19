## Building the server:
```
docker build -t lightswitch:latest .
```

## Running the server:
```
docker run -e "CLIENT_ID=" -e "CLIENT_SECRET=" -e "ACCESS_TOKEN=" -e "REFRESH_TOKEN=" -p 8000:8000 -d lightswitch:latest
```
