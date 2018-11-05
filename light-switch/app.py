import falcon
import os

from wink_api import WinkApi, WinkCredentials

wink_creds = WinkCredentials("light-switch/credentials.json")
wink_api = WinkApi(wink_creds)

api = application = falcon.API()

api.add_route('/wink/{device_type}/{device_id}/{device_action}', wink_api)

