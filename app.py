from flask import Flask, request, render_template, jsonify
import subprocess
import os

app = Flask(__name__)

EXEC_PATH = os.path.join(os.getcwd(), "executables")

@app.route('/')
def index():
    return render_template("index.html")

@app.route('/run', methods=['GET'])
def run_program():
    mode = request.args.get('mode')
    exec_map = {
        'serial': ['serial'],
        'openmp': ['openmp'],
        'mpi': ['mpirun', '-np', '4', 'mpi'],
        'hybrid': ['mpirun', '-np', '4', 'hybrid']
    }

    if mode not in exec_map:
        return jsonify({"error": "Invalid mode"}), 400

    cmd = []
    for part in exec_map[mode]:
        if part in ['mpirun', '-np', '4']:
            cmd.append(part)
        else:
            cmd.append(os.path.join(EXEC_PATH, part))

    try:
        output = subprocess.check_output(cmd, stderr=subprocess.STDOUT).decode()
    except subprocess.CalledProcessError as e:
        output = f"Error:\n{e.output.decode()}"

    return jsonify({"output": output})


if __name__ == "__main__":
    app.run(debug=True)
