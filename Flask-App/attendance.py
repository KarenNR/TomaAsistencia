from flask import Flask, render_template, request, redirect, session, flash
from flask_mysqldb import MySQL

app = Flask(__name__)
app.secret_key = "dae2024"

app.config['MYSQL_HOST'] = 'localhost'
app.config['MYSQL_USER'] = 'root'
app.config['MYSQL_PASSWORD'] = 'karengt'
app.config['MYSQL_DB'] = 'AttendanceSystem'

mysql = MySQL(app)

@app.route('/')
def loadIndex():
    return render_template("index.html")


@app.route('/login', methods=['POST'])
def login():
    _username = request.form['username']
    _password = request.form['password']
    cursor = mysql.connection.cursor()
    cursor.execute('''SELECT Nomina, Nombre, Apellido FROM Profesor WHERE Usuario=(%s) AND Pass=(%s)''', (_username, _password))
    data = cursor.fetchone()
    if data is not None:
        session['nomina'] = data[0]
        session['profesor'] = data[1] + ' ' + data[2]
        return redirect('/clases')
    else:
        flash('El usuario o la contraseña no coinciden.')
        return redirect('/')


@app.route('/clases')
def loadMenu():
    return render_template("profesor/class-menu.html")