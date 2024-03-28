import java.util.HashMap;

public class DNSCache {

    public HashMap<DNSQuestion, DNSRecord> cache_ = new HashMap<>();

    /**
     * Inserts key pair of DNS question and record to cache
     *
     * @param question query of DNS
     * @param record record of DNS
     */
    public void insertRecord (DNSQuestion question, DNSRecord record) {

        if ( !cache_.containsKey(question) ) {
            cache_.put(question, record);
        }
    }

    /**
     * Retrieves a DNS record from the cache from the key DNS question
     * Checks if question is expired and if so removes from cache
     *
     * @param question DNS question key to retrieve record from cache
     * @return DNS record
     */
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
