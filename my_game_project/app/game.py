from flask import Blueprint, jsonify
import ctypes
import os

# Define Flask Blueprint
game_bp = Blueprint('game', __name__)

# Get the absolute path of the DLL
dll_path = os.path.join(os.path.dirname(__file__), "game_backend.dll")

# Load the C shared library
try:
    game_lib = ctypes.CDLL(dll_path)
    game_lib.start_game.argtypes = []  
    game_lib.start_game.restype = None  

except Exception as e:
    print("Error loading C library:", e)
    game_lib = None

@game_bp.route('/start_game', methods=['GET'])
def start_game():
    if game_lib:
        game_lib.start_game()
        return jsonify({'message': 'Game started!'})
    else:
        return jsonify({'error': 'Game library not loaded'}), 500
