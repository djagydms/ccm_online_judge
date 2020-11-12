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
$ cd backend
$ npm install

$ cd ../db
$ sudo docker-compose up -d
$ sudo docker exec -it psql su -c 'psql -d 'userdb' -U 'gydms' -f /tmp/init/init.pgsql'

$ cd ../marker 
$ make

$ cd ../backend
$ sudo node index.js
```

## 언어추가 방법

### Makefile에 규칙 추가
```
/* marker/Makefile */
// X언어 build 규칙 추가
OBJS=$(addprefix $(BUILD_DIR)/, main.o conf.o testcase.o c.o python.o, X.o)
```

### lang.h에 X언어 install 함수 선언
``` c
/* marker/include/lang.h */

struct langsw X_install(); // X언어에 대한 install 함수 선언
static struct langsw (*install[MAX_LANG])(void) = {
		c_install,
		python_install,
		X_install, // X언어를 runtime에 install
		NULL
};
```

### X.c에 X언어의 create, prepare, exec 함수 정의
``` c
/* marker/src/X.c */
int X_create(void *_conf)
{
		struct conf *conf = _conf;

		/* 도커 컨테이너 생성 */

		return exec_cmd(args, NULL, NULL, 0);
}

int X_prepare(void *_conf)
{
		struct conf *conf = _conf;

		/* 소스코드 파일 복사 및 컴파일 */

		return exec_cmd(args, NULL, NULL, 0);
}

int X_exec(void *_conf, struct score *score)
{
		struct conf *conf = _conf;

		/* 테스트케이스를 이용한 채점 */

		return 0;
}

struct langsw X_install()
{
		struct langsw sw = {
				.name = "X",
				.ext = ".x",
				.create = X_create,
				.prepare = X_prepare,
				.exec = X_exec
		}

		return sw;
}

```

## 참여인원 
* 엄효은, djagydms1@gmail.com

## 라이센스
* 
