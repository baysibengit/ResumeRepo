import javax.xml.crypto.Data;
import java.io.*;
import java.util.Arrays;
import java.util.Date;
import java.util.HashMap;

public class DNSRecord {

    private String[] recordName_;
    private int recordType_;
    private int recordClass_;
    private int TTL_;
    private int RDLength_;
    private byte[] RData_;
    private Date creationOfRecord_;
    private long recordTimeStamp_;


    public static DNSRecord decodeRecord(InputStream inputStream, DNSMessage message) throws IOException {

        inputStream.mark(2);//2 to counteract first two bytes for pointer
        DataInputStream dataInputStream = new DataInputStream(inputStream);
        DNSRecord record = new DNSRecord();

        //Read byte to check for message compression
        int firstTwoBytes = dataInputStream.readShort();
        //Extract first two bits
        int compressionPointer = firstTwoBytes & 0xC000;
        //Check if byte is equal to 1 1 signifying a pointer
        if ( compressionPointer ==  0xC000 ) {
            int offset = firstTwoBytes & 0x3FFF;
            record.recordName_ = message.readDomainName(offset);
        }
        else { //Not a pointer
            dataInputStream.reset();
            record.recordName_ = message.readDomainName(inputStream);
        }

        record.recordType_ = dataInputStream.readShort();
        record.recordClass_ = dataInputStream.readShort();
        record.TTL_ = dataInputStream.readInt();
        record.RDLength_ = dataInputStream.readShort();
        record.RData_ = inputStream.readNBytes(record.RDLength_);
        record.creationOfRecord_ = new Date();
        record.recordTimeStamp_ = record.creationOfRecord_.getTime();
        System.out.println(record.toString());

        return record;
    }

    public void writeBytes(ByteArrayOutputStream outputStream, HashMap<String, Integer> hashMap) throws IOException {

        DNSMessage.writeDomainName(outputStream, hashMap, recordName_);
        outputStream.write(recordType_ >> 8);
        outputStream.write((byte)recordType_);

        outputStream.write(recordClass_ >> 8);
        outputStream.write((byte)recordClass_);

        outputStream.write(TTL_ >> 24);
        outputStream.write((byte)(TTL_ >>16));
        outputStream.write((byte)(TTL_ >> 8));
        outputStream.write((byte)(TTL_));

        outputStream.write(RDLength_ >> 8);
        outputStream.write((byte)RDLength_);

        outputStream.write(RData_);

    }


    /**
     * return whether the creation date + the time to live is after the current time. The Date and Calendar classes will be useful for this.
     * @return true if record is expired
     */
    public boolean isExpired() {

        long lifeSpan = recordTimeStamp_ + TTL_;
        Date currentTime = new Date();
        long currentTimeComp = currentTime.getTime();

        return lifeSpan < currentTimeComp;
    }


    @Override
    public String toString() {
        return "DNSRecord{" +
                "recordName_=" + Arrays.toString(recordName_) +
                ", recordType_=" + recordType_ +
                ", recordClass_=" + recordClass_ +
                ", TTL_=" + TTL_ +
                ", RDLength_=" + RDLength_ +
                ", RData_=" + Arrays.toString(RData_) +
                ", creationOfRecord_=" + creationOfRecord_ +
                ", recordTimeStamp_=" + recordTimeStamp_ +
                '}';
    }
}
