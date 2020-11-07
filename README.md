# CCM 온라인 저지

> 제2회 PNU 오픈소스SW개발 경진대회 - 온라인 저지 시스템

## 사용버전

* node.js >= v8.10.0
* express >= ~4.13.1
* Docker >= 19.03.6
* Docker-compose >= 1.17.1
* PostgreSQL >= 10.14

## 설치방법

```
$ cd ccm/backend
$ npm init
$ npm install
$ sudo docker-compose up -d
//db 만들기(init.pgsql 주석 풀어야됨)
$ sudo docker exec -it psql su -c 'psql -d 'userdb' -U 'gydms' -f /tmp/init/init.pgsql'
$ cd ../marker 
$ make
$ sudo node index.js
```

## 참여인원 
* 엄효은, djagydms1@gmail.com

## 라이센스
* 
