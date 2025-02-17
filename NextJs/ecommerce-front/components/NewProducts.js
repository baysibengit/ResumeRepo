import styled from "styled-components"
import Center from "@/components/Center"
import ProductBox from "./ProductBox";
import ProductsGrid from "./ProductsGrid";

const Title = styled.h2`
    font-size: 2rem;
    margin: 30px 0 20px; 
    font-family: 'Poppins', san-serif;
`;

export default function NewProducts({products}) {
    return (
        <Center>
            <Title>New Arrivals</Title>
            <ProductsGrid products={products} />
        </Center>
    )
}