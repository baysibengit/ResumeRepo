import Center from "@/components/Center";
import styled from "styled-components";
import Button from "./Button";
import ButtonLink from "./ButtonLink";
import CartIcon from "./icons/CartIcon";
import { useContext } from "react";
import { CartContext } from "./CartContext";

const Bg = styled.div`
    background-color: #303030;
    color: #fff;
    padding: 50px 0;
`;

const Title = styled.h1`
    margin:0;
    font-weight:normal; 
    font-size: 3rem;
`;

const Desc = styled.p`
    color: #aaa;
    font-size: .8rem;
`;

const ColumnsWrapper = styled.div`
    display: grid; 
    grid-template-columns: .9fr 1.1fr; 
    gap: 40px; 
    img{
        max-width: 100%;
    }
`;

const Column = styled.div`
    display: flex; 
    align-items: center; 
`

const ButtonsWrapper = styled.div`
    display: flex; 
    gap: 10px; 
    margin-top: 25px; 
`;


export default function Featured({product}) {
    const {addProduct} = useContext(CartContext)
    function addFeaturedToCart(){
        addProduct(product._id)
    }
    return (
        <Bg>
            <Center>
                <ColumnsWrapper>
                    <Column>
                    <div>
                        <Title>{product.title}</Title>
                        <Desc> {product.description}</Desc>
                        <ButtonsWrapper>
                            <ButtonLink href={'/products/'+product._id} outline={1} white={1}>Read More</ButtonLink>
                            <Button primary={1} onClick={addFeaturedToCart}>
                                <CartIcon />
                                Add to Cart
                            </Button>
                        </ButtonsWrapper>
                    </div>
                    </Column>
                    <Column>
                        <img 
                            src="https://ben-next-ecommerce.s3.amazonaws.com/1732565882249.png"
                            alt="" />
                    </Column>
                </ColumnsWrapper>
                
            </Center>
        </Bg>
    )
}