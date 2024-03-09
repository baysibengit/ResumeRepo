import java.io.*;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Base64;
import java.util.Map;


public class HTTPResponse {

    /**
     * Members
     */
    private  final HTTPRequest request_;
    private OutputStream outputStream_;
    private Room room;
    private String echoMessage_;
    private String messageType_;
    private String roomname_;
    private String username_;
    private String message_;


    /**
     * Constructor to build an HTTP response
     * Takes in and parses Http request and builds appropriate response
     * @param request HTTP request to respond to
     */
    public HTTPResponse(HTTPRequest request) {

        this.request_ = request;

    }

    /**
     * Parses Regular HTTP request or websocket request.
     * Parses encrypted byte packet, handles possible extensions, and
     *  prepares appropriate response.
     * @throws IOException if unable to open up output stream
     * @throws NoSuchAlgorithmException if unable to encrypt using secure hash algorithm
     */
    public void respond() throws IOException, NoSuchAlgorithmException {

        //Get output stream from client socket
        outputStream_ = request_.getClient().getOutputStream();
        //Wrap output stream in print writer
        PrintWriter pw = new PrintWriter(outputStream_);
        //Create a file object with requested file name as the path
        File file = new File(request_.getRequestedFileName());
        Map< String, String > headerMap = request_.getHeaderMap_();

        if ( !request_.isWebSocketRequest(headerMap) ) { //Regular HTTP

            if (file.exists()) {
                //Sends out success
                pw.print("HTTP/1.1 200 OK\r\n");
            } else {
                fileNotFoundHTTPProtocol(pw, request_);
                return;
            }
            handleHeaderExtensions(request_, pw );
            FileInputStream fileInputStream = new FileInputStream(file);
            //Use transferTo so we do not read it line by line and paste everything
            fileInputStream.transferTo(outputStream_);
            //Close all connections  and flush remaining contents of file
            outputStream_.flush();
            pw.close();
            outputStream_.close();
            request_.getClient().close();
        }

        if ( request_.isWebSocketRequest(headerMap) ) {

            performHandShake(request_.getAcceptString_(), pw );

            //Keep connection open to receive messages
            while (true) {

                //Parse packet and broadcast message
                echoMessage_ = parsePacket(request_);
                sendMessage( echoMessage_ , outputStream_ );
                //Extract message type, room name, and username
                messageType_ = getMessageType();
                roomname_ = getRoomname();
                username_ = getUsername();
                room = Room.getRoom( roomname_ );
                handleMessageType(messageType_);
                //Broadcast message to all users in the room
                room.sendMessageToAllUsers( echoMessage_ );
            } // END OF WHILE-TRUE LOOP

        } // END OF WEB SOCKET STUFF

    } // END OF send()

    /**
     * Handles message type for web chat. Can either be of type join, message, or leave.
     * @param messageType one of the above message types
     * @throws IOException if unable to read from input stream
     */
    public void handleMessageType ( String messageType ) throws IOException {

        if (messageType.equals("message")) {
            message_ = getMessage();
        }
        if ( messageType.equals("join") ) {
            room.addClient( username_, request_.getClient() );
            //Sends all messages upon joins
            for( String message : room.getMessagesList() ) {
                sendMessage( message, outputStream_ );
            }
        }
        if ( messageType.equals("leave") ) {
            room.removeClient(username_, request_.getClient());
        }
    }

    /**
     * Pareses encrypted byte packet to pull out following info:
     * masked bit, payload length, header bytes
     * @param request encrypted byte packet from HTTP request
     * @return decoded message from byte packet
     * @throws IOException if unable to read from input stream
     */
    public String parsePacket( HTTPRequest request ) throws IOException {

        DataInputStream dataInputStream = new DataInputStream(request_.getClient().getInputStream());

        //Extract first two bytes for header
        byte[] headerBytes = dataInputStream.readNBytes(2);
        //Checks for a masked bit
        boolean hasMask = false;
        int maskIdentifier = headerBytes[1] & 0x80;
        if (maskIdentifier != 0) {
        hasMask = true;
        }
        //Length value
        int payloadLength = headerBytes[1];
        if (payloadLength < 126) {
            payloadLength = payloadLength & 0x7F;
        }
        else if (payloadLength == 126) {
            payloadLength = dataInputStream.readShort();
        }
        else {
            payloadLength = (int) dataInputStream.readLong();
        }
        //Initialize byte array to handle is there is a mask
        byte[] decodedMessage;

        if ( hasMask ) {
            decodedMessage = new byte[payloadLength];
            byte[] maskArray = dataInputStream.readNBytes(4);
            //Initialize empty  variable to store decoded value
            byte[] encodedMessage = dataInputStream.readNBytes(payloadLength);
            for ( int i = 0; i < encodedMessage.length; i++ ) {
                decodedMessage[i] = (byte) (encodedMessage[i] ^ maskArray[i % 4]);
            }
        }
        else {
            decodedMessage = dataInputStream.readNBytes( payloadLength );
        }
        System.out.println( "Decoded Message: " + new String( decodedMessage ) ); // DEBUGGING
        return new String(decodedMessage);
    }

    /**
     * Handles different types of header extensions
     * Such as .html, .css, .jpeg, and .jpg
     * @param request HTTP request asking for above requested file types
     * @param pw print writer to write out content types of above extensions
     */
    public void handleHeaderExtensions( HTTPRequest request, PrintWriter pw ){

        //Handle all header types for requested files
        if ( request.getRequestedFileName().endsWith(".html")) {
            pw.print("Content-type: text/html\r\n");
        }
        else if ( request.getRequestedFileName().endsWith(".css")) {
            pw.print("Content-type: text/css\r\n");
        }
        else if ( request.getRequestedFileName().endsWith(".jpeg") || request.getRequestedFileName().endsWith(".jpg")) {
            pw.print("Content-type: image/jpeg\r\n");
        }
        //Send blank line signifying that we are done with headers
        pw.print("\r\n");
        pw.flush();
    }

    /**
     * Performs HTTP file not found protocol
     * @param pw Print writer to write out necessary protocol
     * @param request HTTP request that requested missing file
     * @throws IOException if unable to open output stream
     */
    public void fileNotFoundHTTPProtocol( PrintWriter pw, HTTPRequest request ) throws IOException {

        //Send out error message
        String fileResponseError = "File not found: " + request.getRequestedFileName();
        pw.print("HTTP/1.1 404 Not Found\r\n\r\n" + fileResponseError);
        pw.flush();
        pw.close();
        request.getClient().close();
    }

    /**
     * Performs HTTP handshake protocol
     * @param acceptString String to be encoded for handshake, Uses SHA algorithm for encryption
     * @param pw print writer to write out necessary fields for handshake
     */
    public void performHandShake( String acceptString, PrintWriter pw ){

        //Encode accept string
        MessageDigest md;
        try {
             md = MessageDigest.getInstance("SHA-1");
        } catch (NoSuchAlgorithmException e) {
            throw new RuntimeException(e);
        }
        md.update(acceptString.getBytes());
        String encodedAcceptString = Base64.getEncoder().encodeToString(md.digest());

        //Write out handshake
        pw.print("HTTP/1.1 101 Switching Protocols \r\n");
        pw.print("Upgrade: websocket \r\n");
        pw.print("Connection: Upgrade \r\n");
        pw.print("Sec-WebSocket-Accept: " + encodedAcceptString + "\r\n\r\n");
        pw.flush();


    }

    /**
     * Sends out response message.
     * Uses bitwise operations to write out message in correct format
     * @param message you get after parsing packet
     * @param outputStream stream used to write out your message
     * @throws IOException if unable to access output stream
     */
    public static void sendMessage( String message, OutputStream outputStream ) throws IOException {

        DataOutputStream dataOutputStream = new DataOutputStream( outputStream );
        int messageLength = message.length();

        byte b0 = (byte) 0x81;
        dataOutputStream.writeByte(b0);
        byte b1;

        if (messageLength < 126) {
            b1 = (byte) messageLength;
            dataOutputStream.writeByte(b1);

        } else if (messageLength < Math.pow(2, 16)) {
            dataOutputStream.writeByte(126);
            dataOutputStream.writeShort(messageLength);
        } else {
            dataOutputStream.writeByte(127);
            dataOutputStream.writeLong(messageLength);
        }
        dataOutputStream.writeBytes(message);
        dataOutputStream.flush();
    }

    /**
     * Getters
     */
    public String getMessageType() {

        String roomname = echoMessage_.split("\"type\":\"") [1];
        roomname = roomname.split("\"")[0];
        return roomname;

    }

    public String getUsername() {

        String roomname = echoMessage_.split("\"user\":\"")[1];
        roomname = roomname.split("\"")[0];
        return roomname;

    }

    public String getRoomname() {

        String roomname = echoMessage_.split("\"room\":\"")[1];
        roomname = roomname.split("\"")[0];
        return roomname;

    }

    public String getMessage() {

        String roomname = echoMessage_.split("\"message\":\"")[1];
        roomname = roomname.split("\"")[0];
        return roomname;

    }

} // END OF HTTPResponse CLASS
//   System.out.println( "Thread Id " + String.valueOf(Thread.currentThread().threadId()) );

