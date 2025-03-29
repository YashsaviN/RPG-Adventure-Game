from flask import Flask
from app.game import game_bp  # Importing the game blueprint

app = Flask(__name__)
app.register_blueprint(game_bp, url_prefix='/game')  # Registering it with /game prefix

if __name__ == "__main__":
    print(app.url_map)  # Print all available routes before running
    app.run(debug=True)
