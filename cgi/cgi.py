#!/usr/bin/env python3

from flask import Flask, render_template

app = Flask(__name__)

@app.route('/')
def hello():
    return "Hello Webserv"

@app.route("/index", methods=['GET'])
def index():
    return render_templete("index.html")

if __name__ == '__main__':
    app.run(debug=True)