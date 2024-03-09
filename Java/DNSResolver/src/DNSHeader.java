import java.io.*;

public class DNSHeader {

    private short transactionId_;
    private short flags_;
    private short QDCOUNT_;
    private short ANCOUNT_;
    private short NSCOUNT_;
    private short ARCOUNT_;
    private byte dataCodeCheck_;


    /**
     * read the header from an input stream
     * parses header information and stores each part
     * @param inputStream inputStream
     * @return DNSHeader
     */
    public static DNSHeader decodeHeader(InputStream inputStream) throws IOException {

        DNSHeader header = new DNSHeader();
        DataInputStream dataStream = new DataInputStream(inputStream);

        header.transactionId_ = dataStream.readShort();
        header.flags_ = dataStream.readShort();
        header.dataCodeCheck_ = (byte)(header.flags_ & 0x000F); //tells if valid domain name
        header.QDCOUNT_ = dataStream.readShort();
        header.ANCOUNT_ = dataStream.readShort();
        header.NSCOUNT_ = dataStream.readShort();
        header.ARCOUNT_ = dataStream.readShort();
        System.out.println(header.toString());
        return header;
    }

    /**
     * This will create the header for the response. It will copy some fields from the request
     * @param request coming in information
     * @param response information we are sending out
     * @return response header to send out
     */
    public static DNSHeader buildHeaderForResponse(DNSMessage request, DNSMessage response) {


        response.getHeader_().transactionId_ = request.getHeader_().getTransactionId_();
        response.getHeader_().flags_ = (short) (request.getHeader_().getFlags_() | 0x8000); //set qr to 1
        response.getHeader_().QDCOUNT_ = request.getHeader_().getQDCOUNT_();
        response.getHeader_().ANCOUNT_ = (short)(request.getHeader_().getANCOUNT_() | 0x0001); //set answer count to 1
        response.getHeader_().NSCOUNT_ = request.getHeader_().getNSCOUNT_();
        response.getHeader_().ARCOUNT_ = request.getHeader_().getARCOUNT_();


        return response.getHeader_();
    }

    /**
     * encode the header to bytes to be sent back to the client.
     * The OutputStream interface has methods to write a single byte or an array of bytes.
     * @param outputStream output stream
     */
    public void writeBytes(OutputStream outputStream) throws IOException {

        outputStream.write(getTransactionId_() >> 8);
        outputStream.write((byte)getTransactionId_());

        outputStream.write(getFlags_() >> 8);
        outputStream.write((byte)getFlags_());

        outputStream.write(getQDCOUNT_() >> 8);
        outputStream.write((byte)getQDCOUNT_());


        outputStream.write(getANCOUNT_() >> 8);
        outputStream.write((byte)getANCOUNT_());

        outputStream.write(getNSCOUNT_() >> 8);
        outputStream.write((byte)getNSCOUNT_());

        outputStream.write(getARCOUNT_() >> 8);
        outputStream.write((byte)getARCOUNT_());


    }

    @Override
    public String toString() {
        return "DNSHeader{" +
                "transactionId_=" + transactionId_ +
                ", flags=" + flags_ +
                ", QDCOUNT_=" + QDCOUNT_ +
                ", ANCOUNT_=" + ANCOUNT_ +
                ", NSCOUNT_=" + NSCOUNT_ +
                ", ARCOUNT_=" + ARCOUNT_ +
                '}';
    }

    /**
     * GETTERS
     */
    public short getTransactionId_() {
        return transactionId_;
    }
    public short getFlags_() {
        return flags_;
    }

    public short getQDCOUNT_() {
        return QDCOUNT_;
    }

    public short getANCOUNT_() {
        return ANCOUNT_;
    }

    public short getNSCOUNT_() {
        return NSCOUNT_;
    }

    public short getARCOUNT_() {
        return ARCOUNT_;
    }
    public byte getDataCodeCheck_() {return dataCodeCheck_;}
}
