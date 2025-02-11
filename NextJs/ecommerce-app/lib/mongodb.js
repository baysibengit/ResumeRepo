// This approach is taken from https://github.com/vercel/next.js/tree/canary/examples/with-mongodb
import { MongoClient } from 'mongodb'

if (!process.env.MONGODB_URI ) {
    throw new Error('Invalid/Missing environment variable: "MONGODB_URI"')
}

const uri = process.env.MONGODB_URI 
const options = {}

let client; 
let clientPromise;

if (process.env.NODE_ENV === 'development') {
    // In development mode, use a global variable 
    // is preseved across module reloads caused HMR (Hot Module Replacements)
    if (!global._mongoClientPromise ) {
        client = new MongoClient(uri, options)
        global._mongoClientPromise = client.connect()
    }
    clientPromise = global._mongoClientPromise
} else {
    // In production mode, it's best to not use a a global variable 
    client = new MongoClient(uri, options)
    clientPromise = client.connect()
}

// Export a module-scoped MongoClient promisel By doing this in a 
// separate module the client can be shared across functions. 
export default clientPromise


// Server less 
// Oath 
// Promises 
// MongoDB 
// Next.js 
// Node.js 
// 24 minutes 

// mongodb+srv://benbaysinger0:AZUTBipj9jPbgfa3@cluster0.xmeeu.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0


// Admin: 
// username: benbaysinger0 
// pword:    j8J7T7raGWtzwYjp