-- CREATE EXTENSION pgcrypto;


-- CREATE TABLE problem (
-- p_idx INT PRIMARY KEY,
-- testCnt INT NOT NULL,
-- compileLimit INT,
-- codeSizeLimit INT,
-- created_on TIMESTAMP NOT NULL
-- );


-- CREATE TABLE testcase (
-- idx INT PRIMARY KEY,
-- p_idx INT NOT NULL,
-- input TEXT NOT NULL,
-- output TEXT NOT NULL,
-- created_on TIMESTAMP NOT NULL
-- );

-- CREATE TABLE userlog(
-- p_idx INT PRIMARY KEY,
-- userID TEXT NOT NULL,
-- passCase INT NOT NULL,
-- compileTime INT,
-- codeSize INT,
-- created_on TIMESTAMP NOT NULL
-- );

-- INSERT INTO problem (p_idx,testCnt,compileLimit,codeSizeLimit,created_on) VALUES (
-- '1',
-- '2',
-- '100',
-- '100',
-- '2020-09-23 10:18:30'
-- );

INSERT INTO testcase (p_idx,idx,input,output,created_on) VALUES (
'2',
'3',
'hi',
'hi',
now()
);

INSERT INTO testcase (p_idx,idx,input,output,created_on) VALUES (
'2',
'4',
'hello',
'hello',
now()
);

-- INSERT INTO users (email, username, password, wallet_address, created_on) VALUES (
-- 'eom@pusan.ac.kr',
-- 'eom',
-- crypt('p@ssw0rd', gen_salt('bf')),
-- '0x0000',
-- now()
-- );

-- SELECT username
-- FROM users
-- WHERE email = 'kmk@pusan.ac.kr'
-- AND password = crypt('p@ssw0rd', password);