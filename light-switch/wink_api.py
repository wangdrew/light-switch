import requests
import json
import falcon
import os

WINK_API_URL='https://api.wink.com/'
WINK_OAUTH_URL='https://api.wink.com/oauth2/token'

class WinkApi(object):

    def __init__(self, creds):
        self.creds = creds

    def on_put(self, req, resp, device_type, device_id):
    data = req.stream.read()
        wink_resp = self.update_device_state(device_type, device_id, data)

        resp.status = str(wink_resp.status_code) + ' ' + wink_resp.reason
        resp.content_type = wink_resp.headers['content-type']
        resp.body = wink_resp.text

    def on_get(self, req, resp, device_type, device_id):
        wink_resp = self.get_device(device_type, device_id)

        resp.status = str(wink_resp.status_code) + ' ' + wink_resp.reason
        resp.content_type = wink_resp.headers['content-type']
        resp.body = wink_resp.text

    def update_device_state(self, device_type, device_id, data):
        url = WINK_API_URL + device_type + '/' + device_id + '/desired_state'
        headers = {}
        headers['Authorization'] = 'Bearer ' + self.creds.get_access_token()
        headers['Content-Type'] = 'application/json'
        resp = requests.put(url, data=data, headers=headers)
    if resp.status_code == 401:
        self.creds.refresh_tokens()
        headers['Authorization'] = 'Bearer ' + self.creds.get_access_token()
        resp = requests.put(url, data=data, headers=headers)
    return resp

    def get_device(self, device_type, device_id):
        url = WINK_API_URL + device_type + '/' + device_id
        headers = {}
        headers['Authorization'] = 'Bearer ' + self.creds.get_access_token()
        headers['Content-Type'] = 'application/json'
        resp = requests.get(url, headers=headers)
    if resp.status_code == 401:
        self.creds.refresh_tokens()
        headers['Authorization'] = 'Bearer ' + self.creds.get_access_token()
            resp = requests.get(url, headers=headers)
    return resp

class WinkCredentials(object):
    def __init__(self, path):
        self.path = path
        self.read_credentials()
    
    def get_access_token(self):
        return self.wink_access_token

    def refresh_tokens(self):
        params = {
            "client_id": self.wink_client_id,
            "client_secret": self.wink_client_secret,
            "grant_type": "refresh_token",
            "refresh_token": self.wink_refresh_token
        }
        resp = requests.post(WINK_OAUTH_URL, params=params)

        if resp.status_code >= 400:
            raise RuntimeError("Error trying to refresh the access token. Data: " + resp.text)
        
        self.wink_access_token = resp.json()["data"]["access_token"]
        self.wink_refresh_token = resp.json()["data"]["refresh_token"]
        self.write_credentials()

    def read_credentials(self):
        f = open(self.path, "r")
        creds = json.loads(f.read())
        f.close()
        self.wink_client_id = creds["wink_client_id"]
        self.wink_client_secret = creds["wink_client_secret"]
        self.wink_access_token = creds["wink_access_token"]
        self.wink_refresh_token = creds["wink_refresh_token"]

    def write_credentials(self):
        f = open(self.path, "w")
        f.write(json.dumps({
            "wink_client_id": self.wink_client_id,
            "wink_client_secret": self.wink_client_secret,
            "wink_access_token": self.wink_access_token,
            "wink_refresh_token": self.wink_refresh_token
        }))
        f.close()   

