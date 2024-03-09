import java.io.*;
import java.net.Socket;
import java.util.HashMap;
import java.util.Map;
import java.util.Scanner;


public class HTTPRequest {
    /**
     * Members
     */
    private final Socket client;
    private String requestedFileName = " ";
    private final Map< String, String > headerMap_ = new HashMap<>();
    private Scanner clientInputScanner_;
    private String acceptString_ = "";
    private final String FILE_LOCATION = "./resources/";
    private static final String SEC_WEBSOCKET_KEY = "Sec-WebSocket-Key";
    private static final String DEFAULT_FILE = "/index.html";


    /**
     * Constructor to create an HTTP Request
     * @param client socket for client
     * @throws IOException if not able to open input stream
     */
    public HTTPRequest(Socket client) throws IOException {
        this.client = client;
        // Parse and process the client's  request
        readRequest();
    }

    /**
     * Reads HTTP Requests
     * Opens up an input stream
     * Pareses and stores header information
     * Extracts webSocket key to handle webSocket requests
     */
    private void readRequest() {
        try {
            clientInputScanner_ = new Scanner(client.getInputStream());
        } catch (IOException e) {
            e.printStackTrace();
        }
            boolean gotFileName = false;

            // Read all the headers sent from the client (browser)
            while ( true ) {
                String headerLine = clientInputScanner_.nextLine();

                if (!gotFileName) {
                    handleFirstLineHttpRequest(headerLine, FILE_LOCATION);
                    gotFileName = true;
                }
                else if (headerLine.isEmpty()) {
                    break;
                }
                else {
                    //  handle rest of head header lines and load into map
                    String headerKey = headerLine.split(":")[0];
                    String headerValue = headerLine.split(": ")[1];
                    headerMap_.put(headerKey, headerValue);
                }
            }
            //Store key in string variable
            String keyString = headerMap_.get(SEC_WEBSOCKET_KEY);
            //Hard coded magic string given in slides
            String magicSting = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
            //Concatenate magic string with key string to get accept keyString
            acceptString_ = keyString + magicSting;
    }


    /**
     * Boolean to determine type of request
     * @param headerMap hashmap to store header values
     * @return true map contains key signifying that request is of type webSocket
     */
    public boolean isWebSocketRequest( Map< String, String > headerMap ) {
        return headerMap.containsKey(SEC_WEBSOCKET_KEY);
    }

    /**
     * Handles first line of HTTP Request
     * stores requested file name in member variable
     * @param headerLine string containing headers of request
     * @param fileLocation location of file
     */
    public void handleFirstLineHttpRequest( String headerLine, String fileLocation ) {

        String[] pieces = headerLine.split(" ");
        //GET filename versionHTTP
        String fileName = pieces[1];
        if ( fileName.equals("/")) {
            fileName = DEFAULT_FILE;
        }
        //"index.html" -> /path/to/file/index.html
        //"abc.jpb" -> /path/to/file/abc.jpg
        fileName = fileLocation + fileName;
        requestedFileName = fileName;
    }
    /**
     * Getters
     */
    public String getRequestedFileName() {
        return requestedFileName;
    }
    public String getAcceptString_(){
        return acceptString_;
    }
    public Socket getClient() {
        return client;
    }
    public Map<String, String> getHeaderMap_() {
        return headerMap_;
    }
    public Scanner getClientInputScanner_() {
        return clientInputScanner_;
    }
}
//Iterate and print out elements of header map
//            System.out.println("list of headers:");
//            for (Map.Entry<String,String> entry : headerMap_.entrySet())
//                System.out.println("Key = " + entry.getKey() +
//                        ", Value = " + entry.getValue());
//Print out key
//            System.out.println( "Sec-WebSocket-Key "  + headerMap_.get("Sec-WebSocket-Key"));





