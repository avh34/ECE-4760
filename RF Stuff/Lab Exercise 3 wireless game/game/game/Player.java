package game;

import java.awt.Color;

public class Player extends GameParticle {
	public String playername;
	public Color playercolor;
	public int facing;
	public char accelerating_action;
	
	public Player(String playername, double x, double y, double vx, double vy) {
		super(0, x, y, vx, vy);
		this.playername = playername;
		this.playercolor = Game.gamestate.assign_color();
		this.facing = 0;
		this.accelerating_action = 'N';
	}
}
