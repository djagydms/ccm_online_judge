-- CREATE EXTENSION pgcrypto;

-- DROP TABLE testcase;
-- DROP TABLE problem;
-- DROP TABLE userlog;

-- CREATE TABLE problem (
-- p_idx INT PRIMARY KEY,
-- userID TEXT NOT NULL,
-- explanation TEXT NOT NULL,
-- compileLimit INT NOT NULL,
-- runLimit INT NOT NULL,
-- created_on TIMESTAMP NOT NULL
-- );


-- CREATE TABLE testcase (
-- idx SERIAL PRIMARY KEY,
-- p_idx INT NOT NULL,
-- input TEXT NOT NULL,
-- output TEXT NOT NULL
-- );

-- CREATE TABLE userlog(
-- idx SERIAL PRIMARY KEY,
-- p_idx INT NOT NULL,
-- lang TEXT NOT NULL,
-- userID TEXT NOT NULL,
-- testcase TEXT NOT NULL,
-- compiletime INT,
-- runtime INT NOT NULL,
-- created_on TIMESTAMP NOT NULL
-- );

-- INSERT INTO problem (p_idx,testCnt,compileLimit,codeSizeLimit,created_on) VALUES (
-- '1',
-- '2',
-- '100',
-- '100',
-- '2020-09-23 10:18:30'
-- );

-- INSERT INTO testcase (p_idx,idx,input,output,created_on) VALUES (
-- '2',
-- '3',
-- 'hi',
-- 'hi',
-- now()
-- );

-- INSERT INTO testcase (p_idx,idx,input,output,created_on) VALUES (
-- '2',
-- '4',
-- 'hello',
-- 'hello',
-- now()
-- );
