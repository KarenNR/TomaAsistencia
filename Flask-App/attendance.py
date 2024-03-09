from flask import Flask, render_template
from flask_mysqldb import MySQL

app = Flask(__name__)
app.secret_key = "dae2024"
mysql = MySQL(app)

@app.route('/')
def loadIndex():
    return render_template("index.html")