package game;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.util.ArrayList;

import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;

public class Game {
	
	static GamePanel gamepanel;
	static GameState gamestate;
	static JPanel scoreboard;
	
	public static final int UPDATE_INTERVAL = 10; //time in ms between updates

	public static void main(String[] args) {
		//launch game
		gamestate = new GameState();
		init_graphics();
		SerialConnection.init_serial("COM7");
		int spawn_counter = 0;
		while (true) {
			try {
				Thread.sleep(UPDATE_INTERVAL);
				gamestate.update_game();
				gamepanel.repaint();
				if (spawn_counter++ == 500) {
					gamestate.spawn_particle();
					spawn_counter = 0;
				}
				if (gamestate.game_time_left < 0)
					gamestate = new GameState();
			} catch (InterruptedException e) {
			}
		}
	}
	
	private static void init_graphics() {
		JFrame f = new JFrame("Wireless game");
		GamePanel.load_images();
		f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		f.setSize(1000,800); 
		gamepanel = new GamePanel();
		scoreboard = new JPanel();
		scoreboard.setLayout(new BoxLayout(scoreboard, BoxLayout.Y_AXIS));
		scoreboard.setPreferredSize(new Dimension(1000,100));
		f.add(scoreboard,BorderLayout.NORTH);
		f.add(gamepanel,BorderLayout.SOUTH);
		f.pack();
		f.setVisible(true); 
	}

	protected static void update_score() {
		scoreboard.removeAll();
		scoreboard.add(new JLabel("Scores"));
		for (int i = 0; i < Game.gamestate.number_of_players; i++) {
			JLabel scorelabel = new JLabel(Game.gamestate.corrals[i].playername + "    " + Game.gamestate.corrals[i].score);
			scorelabel.setForeground(Game.gamestate.corrals[i].color);
			scoreboard.add(scorelabel);
		}
		scoreboard.revalidate();
	}
}
