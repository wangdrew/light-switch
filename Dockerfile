FROM python:2.7

ADD light-switch /light-switch
ADD requirements.txt entrypoint.sh /
RUN pip install -r requirements.txt

EXPOSE 8000

CMD "/entrypoint.sh"

