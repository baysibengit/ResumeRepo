import axios from "axios"
import {useRouter} from "next/router"
import {useState, useEffect} from "react"
import Spinner from "./Spinner"
import {ReactSortable} from "react-sortablejs"



export default function ProductForm({
    _id,
    title:existingTitle,
    description:existingDescription,
    price:existingPrice,
    images:existingImages,
    category:assignedCategory,
    properties:assignedProperties,
}) {
    const[title,setTitle] = useState(existingTitle || '')
    const [description,setDescription] = useState(existingDescription || '')
    const [category,setCategory] = useState(assignedCategory ||'')
    const [productProperties,setProductProperties] = useState(assignedProperties || {})
    const [price,setPrice] = useState(existingPrice || '')
    const [images,setImages] = useState(existingImages || [])
    const [goToProducts,setGoToProducts] = useState(false)
    const [isUploading,setIsUploading] = useState(false)
    const router = useRouter()
    const [categories,setCategories] = useState([])
    useEffect( () => {
        axios.get('/api/categories').then(result => {
            setCategories(result.data)
        })
    }, [])
    console.log({_id})

    async function saveProduct(ev) {
        ev.preventDefault()
        const data = {
            title,description,price,images,category,
            properties:productProperties
        }

        if (_id){
            //update
            await axios.put('/api/products', {...data,_id})
        } else {
            //create
            await axios.post('/api/products', data)
        }
        // Redirection
        setGoToProducts(true)
        
        
    }

    if (goToProducts) {
        router.push('/products')
    }

    async function uploadImages(ev) {
        const files = ev.target?.files
        if ( files?.length > 0 ) {
            setIsUploading(true)
            const data = new FormData()
            for ( const file of files ) {
                data.append('file', file)
            }
            const res = await axios.post('/api/upload', data)
            setImages(oldImages => {
                return [...oldImages, ...res.data.links]
            })
            setIsUploading(false)
        }
    }

    function updateImagesOrder(images) {
        setImages(images)
    }

    function setProductProp(propName,value) {
        setProductProperties(prev => {
            const newProductProps = {...prev}
            newProductProps[propName] = value
            return newProductProps
        })
    }

    const propertiesToFill = []
    if (categories.length > 0 &&  category) {
        let catInfo = categories.find(({_id}) => _id === category)
        propertiesToFill.push(...catInfo.properties)
        while(catInfo?.parent?._id) {
            const parentCat = categories.find(({_id}) => _id === catInfo?.parent?._id)
            propertiesToFill.push(...parentCat.properties)
            catInfo = parentCat
        }
    }

    return (
            <form id="form" className="px-2" onSubmit={saveProduct}>
                <label> <b>Card Name</b></label>
                <input 
                    type="text" 
                    placeholder="product name" 
                    value={title} 
                    onChange={ev => setTitle(ev.target.value)}/>
                <label>
                    <b>Category</b> 
                </label>
                <select 
                    value={category} 
                    onChange={ev => setCategory(ev.target.value)}>
                    <option value="">Uncategorized</option>
                    {categories.length > 0 && categories.map(c => (
                        <option value={c._id}>{c.name}</option>
                    ))}
                </select>
                {propertiesToFill.length > 0 && propertiesToFill.map(p => (
                    <div className="flex gap-1 ml-2 text-black">
                      <div className="text-white">{p.name}</div> 
                      <select 
                        value={productProperties[p.name]}
                        onChange={ev => setProductProp(p.name,ev.target.value)}>
                        {p.values.map(v => (
                            <option value={v}>{v}</option>
                        ))}
                      </select>
                    </div>
                ))}
                <label>
                    <b>Photos</b>
                </label>
                <div className="photosDiv">
                    <ReactSortable 
                        list={images} 
                        className="flex flex-wrap gap-1"
                        setList={updateImagesOrder}>
                        {!!images?.length && images.map(link => (
                            <div key={link} className="imageDiv">
                                <img src={link} alt="" className="rounded-lg"></img>
                            </div>
                        ))}
                    </ReactSortable>
                    {isUploading && (
                        <div className="uploading" >
                            <Spinner />
                        </div>
                    )}
                    <label className="imageLabel">
                        <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" strokeWidth={1.5} stroke="currentColor" className="w-6 h-6">
                            <path strokeLinecap="round" strokeLinejoin="round" d="M3 16.5v2.25A2.25 2.25 0 0 0 5.25 21h13.5A2.25 2.25 0 0 0 21 18.75V16.5m-13.5-9L12 3m0 0 4.5 4.5M12 3v13.5" />
                        </svg>
                        <div>
                            Upload
                        </div>
                        <input type="file" 
                            onChange={uploadImages}
                            className="fileInput"/> 
                    </label>
                </div>
                <label> <b>Description</b></label>
                <textarea 
                    placeholder="description"
                    value={description}
                    onChange={ev => setDescription(ev.target.value)}>
                </textarea>
                <label> <b>Price</b></label>
                <input 
                    type="number" 
                    placeholder="price"
                    value={price}
                    onChange={ev => setPrice(ev.target.value)}>
                </input>
                <button 
                    type="submit" 
                    className="btn-primary">
                        <b>Save</b>
                </button>
            </form>
    )
}