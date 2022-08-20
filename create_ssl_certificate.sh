cd ./bin
if [ ! -d "ssl" ]; then
        mkdir -p ssl
fi
cd ./ssl
openssl req -newkey rsa:2048 -nodes -keyout rsa_private.key -x509 -days 365 -out cert.crt -subj "/C=CN/ST=GD/L=SZ/O=LFR/OU=dev/CN=haha_web.com/emailAddress=1107682452@qq.com"