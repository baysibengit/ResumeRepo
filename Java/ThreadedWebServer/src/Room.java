import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;


public class Room {
    /**
     * Members
     */
    private static final HashMap<String, Room > roomsMap_ = new HashMap<>();

    private final String roomName_;

    private final HashMap< String, Socket > usersMap_ = new HashMap<>();

    private final ArrayList<String> messagesList_ = new ArrayList<>();


    /**
     * Constructor to build a room
     * @param roomName name of the room
     */
    private Room(String roomName ) {

        roomName_ = roomName ;

    }

    /**
     * Factory function to return a room if it already exists
     *  if it doesn't it creates the room
     * @param roomName name of the room to be returned/created
     * @return room with the name entered in as a parameter
     */
    public synchronized static Room getRoom( String roomName ) {

         Room room = roomsMap_.get( roomName );
         if (room == null){
             room = new Room( roomName );
             roomsMap_.put(roomName, room);
             return room;
        }
         return room;
    }

    /**
     * Stores all clients/users in a map
     * @param username username to be stored as map key
     * @param socket client to be stored as map value
     */
    public synchronized void addClient( String username, Socket socket ) {

        usersMap_.put( username, socket );

    }

    /**
     * Removes clients/users from map
     * @param username to be removed as map key
     * @param socket client to be removed as map value
     */
    public synchronized void removeClient( String username, Socket socket ) {

        usersMap_.remove( username, socket );

    }

    /**
     * Sends message to all users in a given room
     * @param message to be sent to all users in a room
     * @throws IOException if unable to access output stream
     */
    public synchronized void sendMessageToAllUsers ( String message ) throws IOException {
        //Adding all messages to arraylist ,so I can later send it out when somebody joins
        messagesList_.add( message );

        String myUsername = (message.split("\"user\":\"")[1]).split("\"")[0];;

        for (Map.Entry<String, Socket> entry: usersMap_.entrySet() ){

            if(!myUsername.equals(entry.getKey())) {
                Socket socket = entry.getValue();

                HTTPResponse.sendMessage(message, socket.getOutputStream());
            }

         }
    }


    /**
     * Getter
     * @return list of messages posted in a room
     */
    public ArrayList< String > getMessagesList() {
        //Getter for all messages
        return messagesList_;

    }

    public String getRoomName_() {
        return roomName_;
    }
}
