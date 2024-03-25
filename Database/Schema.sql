CREATE DATABASE AttendanceSystem;

USE AttendanceSystem;

CREATE TABLE Alumno (
    Matricula int PRIMARY KEY,
    Nombre varchar(200),
    Apellido varchar(200),
    Carrera varchar(100)
);

CREATE TABLE Huella_Alumno (
    ID_Huella int PRIMARY KEY AUTO_INCREMENT,
    Matricula int,
    LimInferior int,
    LimSuperior int,
    CONSTRAINT huellaalumno_fk1 FOREIGN KEY (Matricula) REFERENCES Alumno (Matricula)
);

CREATE TABLE Profesor (
    Nomina int PRIMARY KEY,
    Nombre varchar(200),
    Apellido varchar(200),
    Usuario varchar(100),
    Pass varchar(100)
);

CREATE TABLE Clase (
    ID_Clase int PRIMARY KEY AUTO_INCREMENT,
    Nombre varchar(200),
    Salon varchar(100),
    L bool,
    M bool,
    X bool,
    J bool,
    V bool,
    S bool,
    D bool,
    Hora_Inicio time,
    Hora_Final time,
    Nomina_Profesor int,
    CONSTRAINT clase_fk1 FOREIGN KEY (Nomina_Profesor) REFERENCES Profesor (Nomina)
);

CREATE TABLE Alumno_Clase (
    Matricula_Alumno int,
    ID_Clase int,
    PRIMARY KEY (Matricula_Alumno, ID_Clase),
    CONSTRAINT alumnoclase_fk1 FOREIGN KEY (Matricula_Alumno) REFERENCES Alumno (Matricula),
    CONSTRAINT alumnoclase_fk2 FOREIGN KEY (ID_Clase) REFERENCES Clase (ID_Clase)
);

CREATE TABLE Asistencia (
    ID_Clase int,
    Matricula_Alumno int,
    Fecha Date,
    Asistencia int,
    PRIMARY KEY (ID_Clase, Matricula_Alumno, Fecha),
    CONSTRAINT asistencia_fk1 FOREIGN KEY (ID_Clase) REFERENCES Clase (ID_Clase),
    CONSTRAINT asistencia_fk2 FOREIGN KEY (Matricula_Alumno) REFERENCES Alumno (Matricula)
);