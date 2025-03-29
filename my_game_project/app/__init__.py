from flask import Flask

def create_app():
    app = Flask(__name__)

    from app.game import game_bp
    app.register_blueprint(game_bp, url_prefix='/game')

    return app

