package game;

public class GameParticle {
	public int type;
	public double x;
	public double y;
	public double vx;
	public double vy;
	public int hitcounter;
	
	public GameParticle(int type, double x, double y, double vx, double vy) {
		this.type = type;
		this.x = x;
		this.y = y;
		this.vx = vx;
		this.vy = vy;
		this.hitcounter = 0;
	}
}
