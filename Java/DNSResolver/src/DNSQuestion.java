import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Objects;

public class DNSQuestion {

    private String[] QName_;
    private int QType_;
    private int QClass_;


    /**
     * Reads question from input stream and parses to store fields of DNS question
     *
     * @param inputStream stream to update reading in message
     * @param message message to decode
     * @return question object with filled in fields
     * @throws IOException if I/O error occurred while reading from byte stream
     */
    public static DNSQuestion decodeQuestion(InputStream inputStream, DNSMessage message) throws IOException {

        DataInputStream dataInputStream = new DataInputStream(inputStream);
        DNSQuestion question = new DNSQuestion();
        question.QName_ = message.readDomainName(inputStream);
        question.QType_ = dataInputStream.readShort();
        question.QClass_ = dataInputStream.readShort();

        return question;
    }


    /**
     * Write the member fields of DNS question in bytes which will be sent to the client.
     * The hash map is used for us to compress the message.
     *
     * @param outputStream stream to write out bytes
     * @param domainNameLocations hash map of domain names with their locations. Local cache
     * @throws IOException if I/O error occurred while writing to byte stream
     */
    public void writeBytes(ByteArrayOutputStream outputStream, HashMap<String,Integer> domainNameLocations) throws IOException {

        DNSMessage.writeDomainName(outputStream, domainNameLocations , QName_);
        outputStream.write(QType_ >> 8);
        outputStream.write((byte)QType_ );

        outputStream.write(QClass_ >> 8);
        outputStream.write((byte)QClass_);
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        DNSQuestion question = (DNSQuestion) o;
        return QType_ == question.QType_ && QClass_ == question.QClass_ && Arrays.equals(QName_, question.QName_);
    }

    @Override
    public int hashCode() {
        int result = Objects.hash(QType_, QClass_);
        result = 31 * result + Arrays.hashCode(QName_);
        return result;
    }

    @Override
    public String toString() {
        return "DNSQuestion{" +
                "QName_=" + Arrays.toString(QName_) +
                ", QType_=" + QType_ +
                ", QClass_=" + QClass_ +
                '}';
    }
}
