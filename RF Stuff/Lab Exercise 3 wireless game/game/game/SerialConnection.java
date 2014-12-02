package game;
import java.util.Arrays;

import javax.swing.JFrame;
import javax.swing.JPanel;

import jssc.SerialPort;
import jssc.SerialPortEvent;
import jssc.SerialPortEventListener;
import jssc.SerialPortException;

public class SerialConnection {
	
    static SerialPort serialPort;
    static String buffer= "";

    public static void init_serial(String port) {
        serialPort = new SerialPort(port); 
        try {
            serialPort.openPort();//Open port
            serialPort.setParams(9600, 8, 1, 0);//Set params
            int mask = SerialPort.MASK_RXCHAR + SerialPort.MASK_CTS + SerialPort.MASK_DSR;//Prepare mask
            serialPort.setEventsMask(mask);//Set mask
            serialPort.addEventListener(new SerialPortReader());//Add SerialPortEventListener
        }
        catch (SerialPortException ex) {
            System.out.println(ex);
        }
    }

    static class SerialPortReader implements SerialPortEventListener {

        public void serialEvent(SerialPortEvent event) {
            if(event.isRXCHAR()){//If data is available
                if(event.getEventValue() > 0){
                    try {
                    	//byte read[] = serialPort.readBytes(9);
                        byte read[] = serialPort.readBytes(event.getEventValue());
                        buffer += new String(read);
                        int endindex = buffer.indexOf('>');
                        if (endindex > -1) {
                            int beginindex = buffer.indexOf('<');
                        	if (beginindex > -1 && beginindex < endindex) {
                        		String command = buffer.substring(beginindex+1, endindex);
                        		System.out.println(command);
                        		Game.gamestate.handle_command(command);
                        	}
                        	buffer = buffer.substring(endindex+1);
                        	write_serial();
                        }
                    }
                    catch (SerialPortException ex) {
                        System.out.println(ex);
                    }
                }
            }
            else if(event.isCTS()){//If CTS line has changed state
                if(event.getEventValue() == 1){//If line is ON
                    System.out.println("CTS - ON");
                }
                else {
                    System.out.println("CTS - OFF");
                }
            }
            else if(event.isDSR()){///If DSR line has changed state
                if(event.getEventValue() == 1){//If line is ON
                    System.out.println("DSR - ON");
                }
                else {
                    System.out.println("DSR - OFF");
                }
            }
        }
    }

    public static void write_serial() {
    	try {
    		serialPort.writeBytes(Game.gamestate.facing_info.getBytes());
    	} catch (SerialPortException e) {
    		e.printStackTrace();
    	}
    }
}
