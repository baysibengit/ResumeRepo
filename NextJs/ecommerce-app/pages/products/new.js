import ProductForm from "@/components /ProductForm"
import Layout from "@/components /layout"
import { useState } from "react"

export default function NewProduct() {
    return (
        <Layout>
            <h1 className="ml-2"> <b>New Card</b> </h1>
            <ProductForm />
        </Layout>
    )
}