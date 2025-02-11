import { mongooseConnect } from "@/lib/mongoose";
import { Order } from "@/models/Order";
import {buffer} from 'micro'
const stripe = require('stripe')(process.env.STRIPE_SK)

const endpointSecret = "whsec_347738a2d9593d21f0b2c083aa1f067aded2b631ef1dc2c61d20e6eb210f8224";


export default async function handler(req,res) {
    await mongooseConnect();

    const sig = req.headers['stripe-signature'];

    let event;

    try {
        event = stripe.webhooks.constructEvent(await buffer(req), sig, endpointSecret);
    } catch (err) {
        res.status(400).send(`Webhook Error: ${err.message}`);
        return;
    }

    // Handle the event
    switch (event.type) {
        case 'checkout.session.completed':
        // console.log(event.data.object)
        const data = event.data.object;
        const orderId = data.metadata.orderId;
        const paid = data.payment_status === 'paid';
        if (orderId && paid) {
            await Order.findByIdAndUpdate(orderId,{
            paid:true,
            })
        }
        break;
        default:
        console.log(`Unhandled event type ${event.type}`);
    }

    res.status(200).send('ok');
    }

    export const config = {
    api: {bodyParser:false,}
    };

    // valor-warm-warmer-trust
    // acct_1QRTZxIoDxXrZidr