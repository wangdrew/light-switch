#!/bin/bash

echo '{'  > light-switch/credentials.json
echo '"wink_client_id": "'${CLIENT_ID}'",' >> light-switch/credentials.json
echo '"wink_client_secret": "'${CLIENT_SECRET}'",' >> light-switch/credentials.json
echo '"wink_access_token": "'${ACCESS_TOKEN}'",' >> light-switch/credentials.json
echo '"wink_refresh_token": "'${REFRESH_TOKEN}'"' >> light-switch/credentials.json
echo '}' >> light-switch/credentials.json

gunicorn -b 0.0.0.0:8000 light-switch.app

