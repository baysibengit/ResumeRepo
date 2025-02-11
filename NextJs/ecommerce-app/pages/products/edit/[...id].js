import Layout from "@/components /layout";
import {useRouter} from "next/router"
import {useEffect, useState} from "react"
import axios from "axios"
import ProductForm from "@/components /ProductForm";

export default function EditProductPage() {
    const [productInfo, setProductInfo] = useState(null)
    const router = useRouter()
    const {id} = router.query


    useEffect(() => {
        if (!id) {
            return
        }
        axios.get('/api/products?id=' + id).then(response => {
            setProductInfo(response.data)
        }

        )
    }, [id])
    return(
        <Layout>
            <h1 className="ml-1"> <b>Edit Product</b></h1>
            {productInfo && (
                <ProductForm {...productInfo} />

            )}
        </Layout>
    )
}
