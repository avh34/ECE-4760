package game;

import java.awt.BorderLayout;
import java.awt.Polygon;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

import javax.imageio.ImageIO;
import javax.swing.BorderFactory;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;

public class GamePanel extends JPanel {
	
	static BufferedImage cow_image;
	static BufferedImage player_image;
	
	public static void load_images() {
		try {
			cow_image = ImageIO.read(new File("images/spacecow_small.png"));
			player_image = ImageIO.read(new File("images/player.png"));
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	public GamePanel() {
		setBackground(Color.black);
		//setBackground(Color.white);
		setBorder(BorderFactory.createLineBorder(Color.white));
	}
	
	public Dimension getPreferredSize() {
        return new Dimension(GameState.FIELD_X, GameState.FIELD_Y);
    }
	
	protected void paintComponent(Graphics g) {
		super.paintComponent(g);       
		/*
		Polygon poly = new Polygon();
	    poly.addPoint(200,0);
	    poly.addPoint(600,0);
	    poly.addPoint(800,400);
	    poly.addPoint(600,800);
	    poly.addPoint(200,800);
	    poly.addPoint(0,400);
	    g.setColor(Color.WHITE);
	    g.drawPolygon(poly); */
		ArrayList<GameParticle> p = Game.gamestate.p;
		GameParticle particle;
		for (int i = 0; i < p.size(); i++) {
			particle = p.get(i);
			if (particle.type == 0) {
				Player player = (Player) particle; 
				g.setColor(player.playercolor);
				g.fillOval((int)particle.x-GameState.COW_RADIUS, (int)particle.y-GameState.COW_RADIUS, 2*GameState.COW_RADIUS, 2*GameState.COW_RADIUS);
				g.setColor(Color.WHITE);
				g.fillArc((int)particle.x-GameState.COW_RADIUS, (int)particle.y-GameState.COW_RADIUS, 2*GameState.COW_RADIUS, 2*GameState.COW_RADIUS, player.facing-90-15, 30);
			} else if (particle.type == 1)
				g.drawImage(cow_image, (int)particle.x-GameState.COW_RADIUS, (int)particle.y-GameState.COW_RADIUS, null);
		}
		Corral corral;
		for (int i = 0; i < Game.gamestate.number_of_players; i++) {
			corral = Game.gamestate.corrals[i];
			g.setColor(corral.color);
			switch (i) {
			case 0:
				g.fillRect(GameState.FIELD_X-4, GameState.CORRAL_Y, 4, GameState.CORRAL_WIDTH);
				break;
			case 1:
				g.fillRect(0, GameState.CORRAL_Y, 4, GameState.CORRAL_WIDTH);
				break;
			case 2:
				g.fillRect(GameState.CORRAL_X, 0, GameState.CORRAL_WIDTH, 4);
				break;
			case 3:
				g.fillRect(GameState.CORRAL_X2, 0, GameState.CORRAL_WIDTH, 4);
				break;
			case 4:
				g.fillRect(GameState.CORRAL_X, GameState.FIELD_Y-4, GameState.CORRAL_WIDTH, 4);
				break;
			case 5:
				g.fillRect(GameState.CORRAL_X2, GameState.FIELD_Y-4, GameState.CORRAL_WIDTH, 4);
				break;
			}
		}
		g.drawString("Time left "+Game.gamestate.game_time_left/1000, 2, 15);
	}  

}
