package game;

import java.awt.Color;
import java.util.ArrayList;
import java.util.Random;

public class GameState {
	
	public static final int COW_RADIUS = 25;
	public static final int FIELD_X = 1000;
	public static final int FIELD_Y = 500;
	public static final int CORRAL_Y = 150;
	public static final int CORRAL_X = 150;
	public static final int CORRAL_X2 = 650;
	public static final int CORRAL_WIDTH = 200;
	public static final double PLAYER_ACCELERATION = 0.02;
	public static final double PLAYER_COW_DENSITY_RATIO = 4.0;
	public double drag = 0.002;
	
	private Random rand;
	public ArrayList<GameParticle> p;
	public Corral[] corrals;
	public int game_time_left;
	public int number_of_players;
	public String facing_info;
	
	public GameState() {
		p = new ArrayList<GameParticle>();
		rand = new Random();
		game_time_left = 5*60*1000;
		corrals = new Corral[6];
		facing_info = "";
		number_of_players = 0;
	}
	
	public void update_game() {
		double rijx;
		double rijy;
		double rijdotvij;
		double changevix;
		double changeviy;
		String new_facing_info = "";
		game_time_left -= Game.UPDATE_INTERVAL;
		for (int i = 0; i < p.size(); i++) {
			GameParticle p_i = p.get(i); 
			if (p_i.hitcounter == 0) {
				for (int j = i+1; j < p.size(); j++) {
					GameParticle p_j = p.get(j);
					if (p_j.hitcounter == 0) {
						rijx = p_i.x - p_j.x;
						rijy = p_i.y - p_j.y;
						if ((rijx * rijx + rijy * rijy) <= 4 * COW_RADIUS * COW_RADIUS) {//Math.abs(rijx) <= COW_RADIUS*2 && Math.abs(rijy) <= COW_RADIUS*2)
							rijdotvij = rijx * (p_i.vx - p_j.vx) + rijy * (p_i.vy - p_j.vy);
							changevix = rijdotvij * -rijx / (4 * COW_RADIUS * COW_RADIUS);
							changeviy = rijdotvij * -rijy / (4 * COW_RADIUS * COW_RADIUS);
							if (p_i.type == 0 && p_j.type == 1) {
								changevix = changevix * (2 * PLAYER_COW_DENSITY_RATIO) / (PLAYER_COW_DENSITY_RATIO + 1);
								p_i.vx += changevix / PLAYER_COW_DENSITY_RATIO;
								p_i.vy += changeviy / PLAYER_COW_DENSITY_RATIO;
								p_j.vx -= changevix;
								p_j.vy -= changeviy;
							} else if (p_i.type == 1 && p_j.type == 0) {
								changevix = changevix * (2 * PLAYER_COW_DENSITY_RATIO) / (PLAYER_COW_DENSITY_RATIO + 1);
								p_i.vx += changevix;
								p_i.vy += changeviy;
								p_j.vx -= changevix / PLAYER_COW_DENSITY_RATIO;
								p_j.vy -= changeviy / PLAYER_COW_DENSITY_RATIO;
							} else {
								p_i.vx += changevix;
								p_i.vy += changeviy;
								p_j.vx -= changevix;
								p_j.vy -= changeviy;
							}
							p_i.hitcounter = 1;
							p_j.hitcounter = 1;		
							p.set(j, p_j);
						}
					}
				}
			} else {
				p_i.hitcounter--;
			}
			int cow_captured = -1;
			if (p_i.x >= FIELD_X-COW_RADIUS) { //right edge
				if (p_i.type == 1 && corrals[0] != null && p_i.y >= CORRAL_Y && p_i.y <= CORRAL_Y+CORRAL_WIDTH) 
					cow_captured = 0;
				else {	
					p_i.vx = - p_i.vx; 
					p_i.x = FIELD_X-COW_RADIUS; 
				}
			}
			if (p_i.x <= COW_RADIUS) {  //left edge
				if (p_i.type == 1 && corrals[1] != null && p_i.y >= CORRAL_Y && p_i.y <= CORRAL_Y+CORRAL_WIDTH) 
					cow_captured = 1;
				else {
					p_i.vx = - p_i.vx; 
					p_i.x = COW_RADIUS; 
				}
			}
			if (p_i.y <= COW_RADIUS) { //bottom
				if (p_i.type == 1 && corrals[2] != null && p_i.x >= CORRAL_X && p_i.x <= CORRAL_X+CORRAL_WIDTH) cow_captured = 2;
				else if (p_i.type == 1 && corrals[3] != null && p_i.x >= CORRAL_X2 && p_i.x <= CORRAL_X2+CORRAL_WIDTH) cow_captured = 3;
				else {
					p_i.vy = - p_i.vy; 
					p_i.y = COW_RADIUS; 
				}
			}
			if (p_i.y >= FIELD_Y-COW_RADIUS) { //top edge
				if (p_i.type == 1 && corrals[4] != null && p_i.x >= CORRAL_X && p_i.x <= CORRAL_X+CORRAL_WIDTH) cow_captured = 4;
				else if (p_i.type == 1 && corrals[5] != null && p_i.x >= CORRAL_X2 && p_i.x <= CORRAL_X2+CORRAL_WIDTH) cow_captured = 5;
				else {
					p_i.vy = - p_i.vy; 
					p_i.y = FIELD_Y-COW_RADIUS; 
				}
			}
			if (cow_captured != - 1) { //cow went into a corral
				p.remove(i);
				i--;
				corrals[cow_captured].score++;
				Game.update_score();
				continue;
			}
			// new vel = old vel + acceleration*dt (dt=1)
			p_i.vy -= drag * p_i.vy;
			p_i.vx -= drag * p_i.vx;
			// new pos = old pos + velocity*dt (dt=1)
			p_i.x += p_i.vx;
			p_i.y += p_i.vy;
			if (p_i.type == 0) {
				//update facing or velocity
				Player p_i_player = (Player) p_i;
				double radian_facing;
				new_facing_info += "<"+p_i_player.playername+"|"+p_i_player.facing+">";
				switch (p_i_player.accelerating_action) {
				case 'F':
					//forward accelerate
					radian_facing = ((double) p_i_player.facing / 180.0) * Math.PI;
					p_i_player.vy += PLAYER_ACCELERATION * Math.cos(radian_facing);
					p_i_player.vx += PLAYER_ACCELERATION * Math.sin(radian_facing);
					break;
				case 'B':
					//backwards acceleration
					radian_facing = ((double) p_i_player.facing / 180.0) * Math.PI;
					p_i_player.vy -= PLAYER_ACCELERATION * Math.cos(radian_facing);
					p_i_player.vx -= PLAYER_ACCELERATION * Math.sin(radian_facing);
					break;
				}
				p.set(i, p_i_player);
			} else {
				p.set(i, p_i);
			}
		}
		facing_info = new_facing_info;
	}
	
	public void spawn_particle() {
		int randx = rand.nextInt(FIELD_X - 2*COW_RADIUS) + COW_RADIUS;
		int randy = rand.nextInt(FIELD_Y - 2*COW_RADIUS) + COW_RADIUS;
		int rijx;
		int rijy;
		for (int i = 0; i < p.size(); i++) {
			rijx = (int) p.get(i).x - randx;
			rijy = (int) p.get(i).y - randy;
			if (Math.abs(rijx) <= COW_RADIUS*2 && Math.abs(rijy) <= COW_RADIUS*2) {
				return;
			}
		}
		double randvx = rand.nextDouble();
		double randvy = rand.nextDouble();
		p.add(new GameParticle(1, randx, randy, randvx, randvy));
	}
	
	public boolean spawn_player(String playername) {
		if (number_of_players > 5) //max 6 players
			return false;
		int randx = rand.nextInt(FIELD_X - 2*COW_RADIUS) + COW_RADIUS;
		int randy = rand.nextInt(FIELD_Y - 2*COW_RADIUS) + COW_RADIUS;
		int rijx;
		int rijy;
		for (int i = 0; i < p.size(); i++) {
			rijx = (int) p.get(i).x - randx;
			rijy = (int) p.get(i).y - randy;
			if (Math.abs(rijx) <= COW_RADIUS*2 && Math.abs(rijy) <= COW_RADIUS*2) {
				if (p.get(i).type == 1) //another player in that position
					return false;
				else { //cow gets telefragged
					p.remove(i);
					i--;
				}
			}
		}
		Player newplayer = new Player(playername, randx, randy, 0, 0);
		p.add(newplayer);
		corrals[number_of_players - 1] = new Corral(playername, newplayer.playercolor);
		return true;
	}
	
	public void handle_command(String msg) {
		try {
		String playername = msg.substring(0, 6);
        char accelerating_action = msg.charAt(6);
        //char turning_action = msg.charAt(7);
        int Ain = Integer.parseInt(msg.substring(7,10).trim());
        for (int i = 0; i < p.size(); i++) {
        	GameParticle p_i = p.get(i);
        	if (p_i.type == 0) {
        		Player p_i_player = (Player)p_i;
        		if (p_i_player.playername.equals(playername)) {
        			//p_i_player.turning_action = turning_action;
        			p_i_player.facing = Ain * 360 / 255;
        			p_i_player.accelerating_action = accelerating_action;
        			return;
        		}
        	}
        }
        //player not in game yet
        while (spawn_player(playername) == false);
        Game.update_score();
		} catch (Exception e) {
			return;
		}
	}
	
	protected Color assign_color() {
		int num = (number_of_players++);
		switch (num) {
		case 0:
			return Color.GREEN;
		case 1:
			return Color.CYAN;
		case 2:
			return Color.YELLOW;
		case 3:
			return Color.MAGENTA;
		case 4:
			return Color.ORANGE;
		case 5:
			return Color.RED;
		}
		return Color.WHITE;
	}
	
}
