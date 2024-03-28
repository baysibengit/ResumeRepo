import java.io.*;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;

public class DNSMessage {

    private  byte[] completeMessage_;
    private  DNSHeader header_ = new DNSHeader();
    private ArrayList<DNSQuestion> questions_ = new ArrayList<>();
    private ArrayList<DNSRecord> answers_ = new ArrayList<>();
    private ArrayList<DNSRecord> authorityRecords_ = new ArrayList<>();
    private ArrayList<DNSRecord> additionalRecords_ = new ArrayList<>();


    /**
     * Pareses DNS Message as stream of bytes. Stores necessary fields of DNS
     * like questions and records
     *
     * @param bytes stream of bytes containing DNS query
     * @return DNS message object with necessary fields stored in member variables
     * @throws IOException if an I/O error occurs while reading the byte stream
     */
    public static DNSMessage decodeMessage(byte[] bytes) throws IOException {

        DNSMessage message = new DNSMessage();
        //Store entire message
        message.completeMessage_ = Arrays.copyOf(bytes, bytes.length);
        //Wrap in a ByteArrayInputStream
        InputStream inputStream = new ByteArrayInputStream(bytes);
        //Extract and decode header
        message.header_ = DNSHeader.decodeHeader(inputStream);

        //Extract question information.
        for(int i = 0; i < message.header_.getQDCOUNT_(); i++ ) {
           message.questions_.add(DNSQuestion.decodeQuestion(inputStream, message));
        }
        //Extract record information
        for(int i = 0; i < message.header_.getANCOUNT_(); i++ ) {
            message.answers_.add(DNSRecord.decodeRecord(inputStream, message));
        }
        for(int i = 0; i < message.header_.getNSCOUNT_(); i++ ) {
            message.authorityRecords_.add(DNSRecord.decodeRecord(inputStream, message));
        }
        for(int i = 0; i < message.header_.getARCOUNT_(); i++ ) {
            message.additionalRecords_.add(DNSRecord.decodeRecord(inputStream, message));
        }

        return message;
    }


    /**
     * Reads in and stores the pieces of a domain name starting from the current position of the input stream
     *
     * @param inputStream Stream used to read in bytes
     * @return string array containing domain name
     * @throws IOException if I/O error occurred while reading from byte stream
     */
    public String[] readDomainName(InputStream inputStream) throws IOException {
        //If length is 0 return empty arraylist
        //Wrap in data input stream to have byte functionality
        DataInputStream dataStream = new DataInputStream(inputStream);

        //Initialized length that will store domain/extension length
        byte length = dataStream.readByte();


        //Store byte arrays to concatenate later
        ArrayList<byte[]> parts = new ArrayList<>();

        //Keep reading domain name until a 0 octet
        while ( length != 0 ) {

            byte[] queryByte = new byte[length];
            for ( int i = 0; i < length; i++ ) {
            queryByte[i] = dataStream.readByte();
        }

            parts.add(queryByte);
            length = dataStream.readByte();

        }
        String[] domainNameParts = new String[ parts.size() ];

        //Store parts of byte array as strings and store in string array
        for ( int i = 0; i < parts.size(); i++ ) {
            StringBuilder stringBuilder = new StringBuilder();
            for ( byte b: parts.get(i) ) {
                stringBuilder.append((char)b );
            }
            domainNameParts[i] = stringBuilder.toString();
        }
        return domainNameParts;
    }

    /**
     * Same as above, but used when there's compression ,and we need to find the
     * domain from earlier in the message. Start reading from offset
     *
     * @param firstByte offset to begin reading from
     * @return string array of domain name split by the period
     * @throws IOException if I/O error occurred while reading from byte stream
     */
    public String[] readDomainName(int firstByte) throws IOException {

        //Reads in domain name taking into account compression with two byte octet pointer sequence
        ByteArrayInputStream byteStream = new ByteArrayInputStream(completeMessage_, firstByte, completeMessage_.length - firstByte);
        return readDomainName(byteStream);
    }

    /**
     * Build a response based on the request and the answers you intend to send back.
     * Assigns necessary DNS message fields of response.
     *
     * @param request request we receive
     * @param answers DNSRecords we pull from
     * @return response
     */
    public static DNSMessage buildResponse(DNSMessage request, ArrayList<DNSRecord> answers) {

        DNSMessage response = new DNSMessage();
        response.header_ = DNSHeader.buildHeaderForResponse(request, response);
        response.questions_ = request.questions_;
        response.answers_ = answers;
        response.authorityRecords_ = request.authorityRecords_;
        response.additionalRecords_ = request.additionalRecords_;

        return response;
    }

    /**
     * Gets the bytes to put in a packet and send back
     * @return byte array of a message
     * @throws IOException if I/O error occurred while writing to byte stream
     */
    public byte[] toBytes() throws IOException {

        ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
        HashMap<String,Integer> domainLocations = new HashMap<>();

        header_.writeBytes(outputStream);
        for ( DNSQuestion question : questions_) {
            question.writeBytes(outputStream, domainLocations);
        }
        for (DNSRecord record: answers_ ) {
            record.writeBytes(outputStream, domainLocations);
        }
        for (DNSRecord record: authorityRecords_ ) {
            record.writeBytes(outputStream, domainLocations);
        }
        for (DNSRecord record: additionalRecords_ ) {
            record.writeBytes(outputStream, domainLocations);
        }

        return outputStream.toByteArray();
    }

    /**
     * If this is the first time we've seen this domain name in the packet,
     * write it using the DNS encoding (each segment of the domain prefixed with its length, 0 at the end),
     * and add it to the hash map. Otherwise, writes back a pointer to where the domain has been seen previously.
     *
     * @param os output stream
     * @param domainLocations location in hashmap of domain names
     * @param domainPieces sections of a domain name
     * @throws IOException if I/O error occurred while writing to byte stream
     */
    public static void writeDomainName(ByteArrayOutputStream os, HashMap<String,Integer> domainLocations, String[] domainPieces) throws IOException {

        DataOutputStream dataOutputStream  = new DataOutputStream(os);
        DNSMessage message = new DNSMessage();

        String domain = message.joinDomainName(domainPieces);

        int key;

        if ( domainLocations.containsKey(domain) ) {
            //Pull out index
            key = domainLocations.get(domain);
            //Recompress
            short compressedKey = (short)(key | 0xC000);
            //Write out compression
            dataOutputStream.write(compressedKey>>8);
            dataOutputStream.write((byte)compressedKey);
        }
        else {
            //Put domain in map
            domainLocations.put( domain, os.toByteArray().length);
            for ( String pieces: domainPieces ) {
                dataOutputStream.writeByte(pieces.length());
                dataOutputStream.write(pieces.getBytes());
            }
            dataOutputStream.write(0);//signify end of domain name
        }
    }

    /**
     * Join the pieces of a domain name with dots ([ "utah", "edu"] -> "utah.edu" )
     *
     * @param pieces sections of a domain name
     * @return domain name with .
     */
    public String joinDomainName(String[] pieces) {

        System.out.println( "String domain name: " +  String.join(".", pieces));

        return  String.join(".", pieces);

    }

    /**
     *GETTERS
     */
    public DNSHeader getHeader_() {
        return header_;
    }

    public ArrayList<DNSQuestion> getQuestions_() {
        return questions_;
    }

    public ArrayList<DNSRecord> getAnswers_() {
        return answers_;
    }


    @Override
    public String toString() {
        return "DNSMessage{" +
                ", header_=" + header_ +
                ", questions_=" + questions_ +
                ", answers_=" + answers_ +
                ", authorityRecords_=" + authorityRecords_ +
                ", additionalRecords_=" + additionalRecords_ +
                '}';
    }
}
