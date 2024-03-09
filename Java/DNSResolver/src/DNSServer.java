import java.io.IOException;
import java.net.*;

public class DNSServer {

    private final DNSCache cache_;
    private final DatagramSocket socket_;
    private static final int serverPort_ = 8053;
    private static final int maxByteArraySpace_ = 512;
    private static final int googlePort_ = 53;
    private static final String googleHostName_ = "8.8.8.8";

    private DNSServer ( DNSCache cache, int serverPort ) {
        cache_ = cache;

        try {
            socket_ = new DatagramSocket(serverPort);
        } catch (SocketException e) {
            throw new RuntimeException(e);
        }
    }
    

    public static void main(String[] args) throws IOException {


        //Initialize  a server and start listening
        DNSCache cache = new DNSCache();
        DNSServer server = new DNSServer(cache, serverPort_ );
        start(server);

    }

    private static void start( DNSServer server ) throws IOException {

        //Datagram packet to store incoming requests
        byte[] buffer =new byte[maxByteArraySpace_];
        DatagramPacket requestPacket = new DatagramPacket(buffer, buffer.length);


        //Google address to forward to and receive messages from
        InetAddress googleAddress = InetAddress.getByName(googleHostName_);


        while(true) { //Listen in for requests

            //Receive and decode request
            server.socket_.receive(requestPacket);
            DNSMessage dnsMessage =  DNSMessage.decodeMessage(requestPacket.getData());


            //Check if valid question is stored in cache
            for (int i = 0; i < dnsMessage.getQuestions_().size(); i++ ) {
                if (server.cache_.queryQuestion(dnsMessage.getQuestions_().get(i)) != null) {

                    server.respondToClient(dnsMessage, server.socket_, requestPacket);
                }
                else { //Not contained in cache

                    DatagramPacket googlePacket = server.fetchDataFromGoogle(server.socket_, buffer, requestPacket, googleAddress);
                    //Decode response from google
                    DNSMessage googleMessage = DNSMessage.decodeMessage(googlePacket.getData());


                    //Handle potentially bad domain
                    if (invalidDomainName(googleMessage)) {

                        server.handleError(googleMessage, requestPacket, server.socket_);

                    } else {

                        //Insert question and answer to cache and forward response from Google
                        server.cache_.insertRecord(googleMessage.getQuestions_().get(i), googleMessage.getAnswers_().get(i));
                        server.sendGoogleResponseToClient(googleMessage, requestPacket, server.socket_);

                    }
                }
            }
        }

    }

    private DatagramPacket fetchDataFromGoogle( DatagramSocket socket, byte[] buffer, DatagramPacket packet,  InetAddress googleAddress ) throws IOException {

        //Initialize a datagram packet and send to google
        DatagramPacket googleRequest = new DatagramPacket( buffer, packet.getLength(), googleAddress, googlePort_ );
        socket.send(googleRequest);

        //Receive response form google
        byte[] googleBuffer = new byte[maxByteArraySpace_];
        DatagramPacket googlePacket = new DatagramPacket(googleBuffer, googleBuffer.length);
        socket.receive(googlePacket);

        return googlePacket;
    }

    private void respondToClient( DNSMessage message, DatagramSocket socket, DatagramPacket requestPacket ) throws IOException {

        //Build response and send to client
        DNSMessage response = DNSMessage.buildResponse(message, message.getAnswers_());
        byte[] responseBytes = response.toBytes();
        DatagramPacket responsePacket = new DatagramPacket(responseBytes, responseBytes.length, requestPacket.getAddress(), requestPacket.getPort());
        socket.send(responsePacket);
    }

    private void handleError(DNSMessage googleResponse, DatagramPacket requestPacket, DatagramSocket socket ) throws IOException {

        byte[] errorMessage = googleResponse.toBytes();
        DatagramPacket errorPacket = new DatagramPacket(errorMessage, errorMessage.length, requestPacket.getAddress(), requestPacket.getPort());
        socket.send(errorPacket);
    }

    private void sendGoogleResponseToClient(DNSMessage googleMessage, DatagramPacket requestPacket, DatagramSocket socket) throws IOException {

        //Build response and send to client
        DNSMessage googleResponse = DNSMessage.buildResponse(googleMessage, googleMessage.getAnswers_());
        byte[] responseBuffer = googleResponse.toBytes();
        DatagramPacket googleResponsePacket = new DatagramPacket(responseBuffer, responseBuffer.length, requestPacket.getAddress(), requestPacket.getPort());
        socket.send(googleResponsePacket);
    }
    private static boolean invalidDomainName ( DNSMessage message ) {
        return message.getHeader_().getDataCodeCheck_() == (byte)0b0011;
    }
}



