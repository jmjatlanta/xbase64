# HELP;

USE /opt/1Tdata/xbase/xbase.30/data;

DROP TABLE IF EXISTS TestSqlM.DBF;

CREATE TABLE TestSqlM.DBF( CHRFLD CHAR(6), DTFLD DATE, INTFLD INTEGER, SMINTFLD SMALLINT, NUMFLD NUMERIC(9,4), DECFLD DECIMAL(9,4), FLTFLD FLOAT(9,4)  LGFLD LOGICAL ); 

# CREATE INDEX TestSqlM ON TestSqlM.DBF( CHRFLD );



