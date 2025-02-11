import styled from "styled-components"
import Button from "./Button";
import CartIcon from "./icons/CartIcon";
import Link from "next/link";
import { CartContext } from "./CartContext";
import { useContext } from "react";

const ProductWrapper = styled.div`
`;

const WhiteBox = styled(Link)`
    background-color: #fff;
    padding: 20px;
    height: 120px;
    text-align: center; 
    display: flex; 
    align-items: center;
    justify-content: center;
    border-radius: 10px;
    img{
        max-width: 100%;
        max-height: 150px;
    }
`;

const Title = styled(Link)`
    font-weight: normal; 
    font-size: 1rem;
    margin: 0; 
    color: inherit; 
    text-decoration: none; 
`;

const ProductInfoBox = styled.div`
    margin-top: 5px;
`;

const PriceRow = styled.div`
    display: flex;
    align-items: center; 
    justify-content: space-between;
    margin-top: 2px;
`;

const Price = styled.div`
    font-size: 1.5rem;
    font-weight: 600; 
`;

export default function ProductBox({_id,title,description,price,images}) {
   
    const uri = '/product/'+_id
    const {addProduct} = useContext(CartContext)

    return (
        <ProductWrapper>
            <WhiteBox href={uri}>
                <div>
                    <img src={images?.[0]} alt="" />
                </div>
            </WhiteBox>
            <ProductInfoBox>
                <Title href={uri}>{title}</Title>
                <PriceRow>
                    <Price>
                        ${price}
                    </Price>
                    <Button 
                        onClick={() => addProduct(_id)}
                        primary={1} 
                        outline={1}>
                        <CartIcon />
                    </Button>
                </PriceRow>                
            </ProductInfoBox>
        </ProductWrapper>
       
    )
}