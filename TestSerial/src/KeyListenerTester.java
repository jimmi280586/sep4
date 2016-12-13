
import java.awt.Button;
import java.awt.Dimension;
import java.awt.TextField;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.util.List;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.WindowConstants;

import dk.thibaut.serial.SerialPort;

public class KeyListenerTester extends JFrame implements KeyListener {

    JLabel label;
    TextField text;
    
	SerialConnection serialConnection;
	SerialConnectionReader serialConnectionReader;
	
	
    public KeyListenerTester(String s) {
        super(s);
        super.setDefaultCloseOperation(EXIT_ON_CLOSE);
        
        
        JPanel p = new JPanel();
        
        label = new JLabel("Key Listener!");
    
       
        p.add(label);
            
        add(p);
        addKeyListener(this);
        setSize(300, 150);
        setVisible(true);
       
        List<String> a = SerialPort.getAvailablePortsNames(); 
        System.out.println(a.get(0));
        serialConnection = new SerialConnection(a.get(0));
        serialConnectionReader = serialConnection.getSerialConnectionReader();
        
        serialConnectionReader.printMSG();
            
       
		

    }

    @Override
    public void keyTyped(KeyEvent e) {

        if (e.getKeyCode() == KeyEvent.VK_UP) {
            //System.out.println("up key typed");
        }
        if (e.getKeyCode() == KeyEvent.VK_DOWN) {
            //System.out.println("down key typed");
        }

    }

    @Override
    public void keyPressed(KeyEvent e) {

        if (e.getKeyCode() == KeyEvent.VK_UP) {
            System.out.println("up key pressed");
            serialConnection.write("awa".getBytes());
            serialConnectionReader.printMSG();
      
    		
        }
        if (e.getKeyCode() == KeyEvent.VK_DOWN) {
           System.out.println("down key pressed");
            serialConnection.write("asa".getBytes());
            serialConnectionReader.printMSG();
      
        }

    }

    @Override
    public void keyReleased(KeyEvent e) {
        if (e.getKeyCode() == KeyEvent.VK_UP) {
           // System.out.println("up key Released");
        }
        if (e.getKeyCode() == KeyEvent.VK_DOWN) {
           // System.out.println("down key Released");
        }
    }

    public static void main(String[] args) {
        new KeyListenerTester("Ping Pong Game");
    }
}