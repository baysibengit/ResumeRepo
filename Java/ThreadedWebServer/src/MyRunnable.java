import java.io.IOException;
import java.net.Socket;
import java.security.NoSuchAlgorithmException;

public class MyRunnable implements Runnable{

    /**
     * Member
     */
    private final Socket socket_;

    /**
     * Constructs a new {@code MyRunnable} with the specified Socket.
     * @param socket the socket representing the client connection
     */
    public MyRunnable(Socket socket) {

        socket_ = socket;

    }

    /**
     * The main execution logic of the runnable.
     */
    @Override
    public void run() {

            try {

                HTTPRequest request = new HTTPRequest( socket_ );

                HTTPResponse response = new HTTPResponse( request );

                response.respond();

            } catch (IOException | NoSuchAlgorithmException e) {

                throw new RuntimeException(e);

            }

        }

    }
