/**
 * Author: Ben Baysinger
 * Web Chat Client
 */

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;

public class Main {

    public static void main(String[] args)  {

        //Socket that listens in on port 8080
        ServerSocket server = null;
        try {
            server = new ServerSocket(8080);


        //listen for a connection while the program is running ( server is up )
        while (true) {

            //Create a Socket if the server accepts a request
            Socket client = server.accept();

            //create a Thread using the Socket
            Thread thread = new Thread( new MyRunnable( client ) );

            //start the Thread
            thread.start();

        }
        } catch (IOException e) {
            System.err.println("Error in the server: " + e.getMessage());
            e.printStackTrace();
        }
    }
}