import java.util.HashMap;

public class DNSCache {

    public HashMap<DNSQuestion, DNSRecord> cache_ = new HashMap<>();

    public void insertRecord (DNSQuestion question, DNSRecord record) {

        if ( !cache_.containsKey(question) ) {
            cache_.put(question, record);
        }
    }

    public DNSRecord queryQuestion(DNSQuestion question ) {
        DNSRecord answer = cache_.get(question);
        if ( cache_.containsKey(question) && !answer.isExpired()) {
            return cache_.get(question);
        }

        if ( cache_.containsKey(answer) &&  answer.isExpired() ) {
            cache_.remove(question);
            return null;
        }
        return null;
    }

    @Override
    public String toString() {
        return "DNSCache{" +
                "cache_=" + cache_ +
                '}';
    }
}
