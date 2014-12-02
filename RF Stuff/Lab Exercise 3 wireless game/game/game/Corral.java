package game;

import java.awt.Color;

public class Corral {
	public String playername;
	public int score;
	public Color color;
	
	public Corral(String playername, Color color) {
		this.playername = playername;
		this.score = 0;
		this.color = color;
	}
}
